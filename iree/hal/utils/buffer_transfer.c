// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree/hal/utils/buffer_transfer.h"

#include "iree/base/tracing.h"

//===----------------------------------------------------------------------===//
// iree_hal_device_transfer_range implementations
//===----------------------------------------------------------------------===//

IREE_API_EXPORT iree_status_t iree_hal_device_submit_transfer_range_and_wait(
    iree_hal_device_t* device, iree_hal_transfer_buffer_t source,
    iree_device_size_t source_offset, iree_hal_transfer_buffer_t target,
    iree_device_size_t target_offset, iree_device_size_t data_length,
    iree_hal_transfer_buffer_flags_t flags, iree_timeout_t timeout) {
  // If the source and target are both mappable into host memory (or are host
  // memory) then we can use the fast zero-alloc path. This may actually be
  // slower than doing a device queue transfer depending on the size of the data
  // and where the memory lives. For example, if we have two device buffers in
  // device-local host-visible memory we'd be performing the transfer by pulling
  // all the memory to the CPU and pushing it back again.
  // TODO(benvanik): check for device-local -> device-local and avoid mapping.
  bool is_source_mappable =
      !source.device_buffer ||
      (iree_all_bits_set(iree_hal_buffer_memory_type(source.device_buffer),
                         IREE_HAL_MEMORY_TYPE_HOST_VISIBLE) &&
       iree_all_bits_set(iree_hal_buffer_allowed_usage(source.device_buffer),
                         IREE_HAL_BUFFER_USAGE_MAPPING));
  bool is_target_mappable =
      !target.device_buffer ||
      (iree_all_bits_set(iree_hal_buffer_memory_type(target.device_buffer),
                         IREE_HAL_MEMORY_TYPE_HOST_VISIBLE) &&
       iree_all_bits_set(iree_hal_buffer_allowed_usage(target.device_buffer),
                         IREE_HAL_BUFFER_USAGE_MAPPING));
  if (is_source_mappable && is_target_mappable) {
    return iree_hal_device_transfer_mappable_range(
        device, source, source_offset, target, target_offset, data_length,
        flags, timeout);
  }

  // If the source is a host buffer under 64KB then we can do a more efficient
  // (though still relatively costly) update instead of needing a staging
  // buffer.
  if (!source.device_buffer && target.device_buffer &&
      data_length <= IREE_HAL_COMMAND_BUFFER_MAX_UPDATE_SIZE) {
    const iree_hal_transfer_command_t transfer_command = {
        .type = IREE_HAL_TRANSFER_COMMAND_TYPE_UPDATE,
        .update =
            {
                .source_buffer = source.host_buffer.data,
                .source_offset = source_offset,
                .target_buffer = target.device_buffer,
                .target_offset = target_offset,
                .length = data_length,
            },
    };
    return iree_hal_device_transfer_and_wait(device, /*wait_semaphore=*/NULL,
                                             /*wait_value=*/0ull, 1,
                                             &transfer_command, timeout);
  }

  iree_status_t status = iree_ok_status();

  // Allocate the staging buffer for upload to the device.
  iree_hal_buffer_t* source_buffer = source.device_buffer;
  if (!source_buffer) {
    // Allocate staging memory with a copy of the host data. We only initialize
    // the portion being transferred.
    // TODO(benvanik): use import if supported to avoid the allocation/copy.
    // TODO(benvanik): make this device-local + host-visible? can be better for
    // uploads as we know we are never going to read it back.
    const iree_hal_buffer_params_t source_params = {
        .type = IREE_HAL_MEMORY_TYPE_HOST_LOCAL |
                IREE_HAL_MEMORY_TYPE_DEVICE_VISIBLE,
        .usage = IREE_HAL_BUFFER_USAGE_TRANSFER | IREE_HAL_BUFFER_USAGE_MAPPING,
    };
    status = iree_hal_allocator_allocate_buffer(
        iree_hal_device_allocator(device), source_params, data_length,
        iree_make_const_byte_span(source.host_buffer.data + source_offset,
                                  data_length),
        &source_buffer);
    source_offset = 0;
  }

  // Allocate the staging buffer for download from the device.
  iree_hal_buffer_t* target_buffer = target.device_buffer;
  if (!target_buffer) {
    // Allocate uninitialized staging memory for the transfer target.
    // We only allocate enough for the portion we are transfering.
    // TODO(benvanik): use import if supported to avoid the allocation/copy.
    const iree_hal_buffer_params_t target_params = {
        .type = IREE_HAL_MEMORY_TYPE_HOST_LOCAL |
                IREE_HAL_MEMORY_TYPE_DEVICE_VISIBLE,
        .usage = IREE_HAL_BUFFER_USAGE_TRANSFER | IREE_HAL_BUFFER_USAGE_MAPPING,
    };
    status = iree_hal_allocator_allocate_buffer(
        iree_hal_device_allocator(device), target_params, data_length,
        iree_const_byte_span_empty(), &target_buffer);
    target_offset = 0;
  }

  // Issue synchronous device copy.
  if (iree_status_is_ok(status)) {
    const iree_hal_transfer_command_t transfer_command = {
        .type = IREE_HAL_TRANSFER_COMMAND_TYPE_COPY,
        .copy =
            {
                .source_buffer = source_buffer,
                .source_offset = source_offset,
                .target_buffer = target_buffer,
                .target_offset = target_offset,
                .length = data_length,
            },
    };
    status = iree_hal_device_transfer_and_wait(device, /*wait_semaphore=*/NULL,
                                               /*wait_value=*/0ull, 1,
                                               &transfer_command, timeout);
  }

  // Read back the staging buffer into memory, if needed.
  if (iree_status_is_ok(status) && !target.device_buffer) {
    status = iree_hal_buffer_map_read(target_buffer, 0, target.host_buffer.data,
                                      data_length);
  }

  // Discard staging buffers, if they were required.
  if (!source.device_buffer) iree_hal_buffer_release(source_buffer);
  if (!target.device_buffer) iree_hal_buffer_release(target_buffer);

  return status;
}

IREE_API_EXPORT iree_status_t iree_hal_device_transfer_mappable_range(
    iree_hal_device_t* device, iree_hal_transfer_buffer_t source,
    iree_device_size_t source_offset, iree_hal_transfer_buffer_t target,
    iree_device_size_t target_offset, iree_device_size_t data_length,
    iree_hal_transfer_buffer_flags_t flags, iree_timeout_t timeout) {
  iree_status_t status = iree_ok_status();

  iree_hal_buffer_mapping_t source_mapping = {{0}};
  if (iree_status_is_ok(status)) {
    if (source.device_buffer) {
      status = iree_hal_buffer_map_range(
          source.device_buffer, IREE_HAL_MAPPING_MODE_SCOPED,
          IREE_HAL_MEMORY_ACCESS_READ, source_offset, data_length,
          &source_mapping);
    } else {
      source_mapping = (iree_hal_buffer_mapping_t){
          .contents = source.host_buffer,
      };
    }
  }

  iree_hal_buffer_mapping_t target_mapping = {{0}};
  if (iree_status_is_ok(status)) {
    if (target.device_buffer) {
      status = iree_hal_buffer_map_range(
          target.device_buffer, IREE_HAL_MAPPING_MODE_SCOPED,
          IREE_HAL_MEMORY_ACCESS_DISCARD_WRITE, target_offset, data_length,
          &target_mapping);
    } else {
      target_mapping = (iree_hal_buffer_mapping_t){
          .contents = target.host_buffer,
      };
    }
  }

  iree_device_size_t adjusted_data_length = 0;
  if (iree_status_is_ok(status)) {
    // Adjust the data length based on the min we have.
    if (data_length == IREE_WHOLE_BUFFER) {
      // Whole buffer copy requested - that could mean either, so take the min.
      adjusted_data_length = iree_min(source_mapping.contents.data_length,
                                      target_mapping.contents.data_length);
    } else {
      // Specific length requested - validate that we have matching lengths.
      IREE_ASSERT_EQ(source_mapping.contents.data_length,
                     target_mapping.contents.data_length);
      adjusted_data_length = target_mapping.contents.data_length;
    }

    // Perform the copy, assuming there's anything to do.
    if (adjusted_data_length != 0) {
      memcpy(target_mapping.contents.data, source_mapping.contents.data,
             adjusted_data_length);
    }
  }

  if (source.device_buffer) {
    status =
        iree_status_join(status, iree_hal_buffer_unmap_range(&source_mapping));
  }
  if (target.device_buffer) {
    if (adjusted_data_length > 0 &&
        !iree_all_bits_set(iree_hal_buffer_memory_type(target.device_buffer),
                           IREE_HAL_MEMORY_TYPE_HOST_COHERENT)) {
      status = iree_status_join(
          status, iree_hal_buffer_flush_range(&target_mapping, 0,
                                              adjusted_data_length));
    }
    status =
        iree_status_join(status, iree_hal_buffer_unmap_range(&target_mapping));
  }
  return status;
}

//===----------------------------------------------------------------------===//
// iree_hal_buffer_map_range implementations
//===----------------------------------------------------------------------===//

typedef struct iree_hal_emulated_buffer_mapping_t {
  iree_hal_buffer_t* host_local_buffer;
  iree_hal_buffer_mapping_t host_local_mapping;
} iree_hal_emulated_buffer_mapping_t;

IREE_API_EXPORT iree_status_t iree_hal_buffer_emulated_map_range(
    iree_hal_device_t* device, iree_hal_buffer_t* buffer,
    iree_hal_mapping_mode_t mapping_mode,
    iree_hal_memory_access_t memory_access,
    iree_device_size_t local_byte_offset, iree_device_size_t local_byte_length,
    iree_hal_buffer_mapping_t* mapping) {
  IREE_ASSERT_ARGUMENT(device);
  IREE_ASSERT_ARGUMENT(buffer);
  IREE_ASSERT_ARGUMENT(mapping);

  iree_hal_allocator_t* device_allocator = iree_hal_device_allocator(device);
  iree_allocator_t host_allocator = iree_hal_device_host_allocator(device);

  // We can't perform persistent mapping with this as we need to manage the
  // staging buffer lifetime.
  if (IREE_UNLIKELY(mapping_mode == IREE_HAL_MAPPING_MODE_PERSISTENT)) {
    return iree_make_status(
        IREE_STATUS_INVALID_ARGUMENT,
        "emulated buffer mapping only possible with scoped mappings");
  }

  // No implementation should be using this emulated method with memory that is
  // allocated as mappable.
  if (IREE_UNLIKELY(iree_all_bits_set(iree_hal_buffer_memory_type(buffer),
                                      IREE_HAL_BUFFER_USAGE_MAPPING))) {
    return iree_make_status(
        IREE_STATUS_FAILED_PRECONDITION,
        "emulated buffer mapping should not be used with mappable buffers");
  }

  IREE_TRACE_ZONE_BEGIN(z0);
  IREE_TRACE_ZONE_APPEND_VALUE(z0, (uint64_t)local_byte_length);

  // NOTE: this is assuming that the host is going to be doing a lot of work
  // on the mapped memory and wants read/write caching and such. If the user
  // wants write combining on device memory and other things they should ensure
  // this emulated mapping path is not hit.

  // Create a transient struct we use to track the emulated operation.
  // We could pack this into the mapping but this composes better - it's small
  // and pooled by the host allocator anyway.
  iree_hal_emulated_buffer_mapping_t* emulation_state = NULL;
  IREE_RETURN_AND_END_ZONE_IF_ERROR(
      z0, iree_allocator_malloc(host_allocator, sizeof(*emulation_state),
                                (void**)&emulation_state));

  // Allocate the buffer we'll be using to stage our copy of the device memory.
  // All devices should be able to satisfy this host-local + mapping request.
  iree_status_t status = iree_hal_allocator_allocate_buffer(
      device_allocator,
      (iree_hal_buffer_params_t){
          .type = IREE_HAL_MEMORY_TYPE_HOST_LOCAL,
          .usage =
              IREE_HAL_BUFFER_USAGE_TRANSFER | IREE_HAL_BUFFER_USAGE_MAPPING,
      },
      local_byte_length, iree_const_byte_span_empty(),
      &emulation_state->host_local_buffer);

  // We need to capture a copy of the device buffer to work with; unless the
  // user was nice and said they don't care about the contents with the DISCARD
  // bit. Ideally we'd also enable invalidate_range to specify subranges we want
  // to map.
  if (iree_status_is_ok(status) &&
      !iree_all_bits_set(memory_access, IREE_HAL_MEMORY_ACCESS_DISCARD)) {
    // Download (device->host) the data.
    status = iree_hal_device_transfer_range(
        device, iree_hal_make_device_transfer_buffer(mapping->buffer),
        local_byte_offset,
        iree_hal_make_device_transfer_buffer(
            emulation_state->host_local_buffer),
        0, local_byte_length, IREE_HAL_TRANSFER_BUFFER_FLAG_DEFAULT,
        iree_infinite_timeout());
  }

  if (iree_status_is_ok(status)) {
    // Map the scratch buffer: map-ception.
    // Code-wise it looks like this may loop back onto this emulated path
    // but no implementation should be using this emulation if they have host
    // local IREE_HAL_BUFFER_USAGE_MAPPING memory - and we check that above.
    status = iree_hal_buffer_map_range(emulation_state->host_local_buffer,
                                       IREE_HAL_MAPPING_MODE_SCOPED,
                                       memory_access, 0, local_byte_length,
                                       &emulation_state->host_local_mapping);
  }

  // Retain the scratch buffer for the duration of the mapping.
  if (iree_status_is_ok(status)) {
    // Note that we are giving back the host-local mapped contents to the user -
    // they don't need to know it's from our staging buffer.
    mapping->contents = emulation_state->host_local_mapping.contents;
    mapping->impl.reserved[0] = (uint64_t)((uintptr_t)emulation_state);
  } else {
    status = iree_status_join(
        status,
        iree_hal_buffer_unmap_range(&emulation_state->host_local_mapping));
    iree_hal_buffer_release(emulation_state->host_local_buffer);
    iree_allocator_free(host_allocator, emulation_state);
  }
  IREE_TRACE_ZONE_END(z0);
  return status;
}

IREE_API_EXPORT iree_status_t iree_hal_buffer_emulated_unmap_range(
    iree_hal_device_t* device, iree_hal_buffer_t* buffer,
    iree_device_size_t local_byte_offset, iree_device_size_t local_byte_length,
    iree_hal_buffer_mapping_t* mapping) {
  IREE_ASSERT_ARGUMENT(device);
  IREE_ASSERT_ARGUMENT(buffer);
  IREE_ASSERT_ARGUMENT(mapping);
  IREE_TRACE_ZONE_BEGIN(z0);
  IREE_TRACE_ZONE_APPEND_VALUE(z0, (uint64_t)local_byte_length);
  iree_hal_emulated_buffer_mapping_t* emulation_state =
      (iree_hal_emulated_buffer_mapping_t*)((uintptr_t)
                                                mapping->impl.reserved[0]);
  IREE_ASSERT_NE(emulation_state, NULL);

  // Unmap the scratch buffer first to make it available for copying (if
  // needed).
  iree_status_t status =
      iree_hal_buffer_unmap_range(&emulation_state->host_local_mapping);

  // If we were writing then we'll need to flush the range.
  // Ideally we'd keep track of this on the mapping itself based on the user's
  // calls to flush_range to limit how much we need to transfer.
  if (iree_status_is_ok(status) &&
      iree_all_bits_set(mapping->impl.allowed_access,
                        IREE_HAL_MEMORY_ACCESS_WRITE)) {
    // Upload (host->device) the data.
    status = iree_hal_device_transfer_range(
        device,
        iree_hal_make_device_transfer_buffer(
            emulation_state->host_local_buffer),
        0, iree_hal_make_device_transfer_buffer(mapping->buffer),
        local_byte_offset, local_byte_length,
        IREE_HAL_TRANSFER_BUFFER_FLAG_DEFAULT, iree_infinite_timeout());
  }

  // Deallocate the scratch buffer and our emulation state.
  iree_hal_buffer_release(emulation_state->host_local_buffer);
  iree_allocator_t host_allocator = iree_hal_device_host_allocator(device);
  iree_allocator_free(host_allocator, emulation_state);

  IREE_TRACE_ZONE_END(z0);
  return status;
}
