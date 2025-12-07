#include "utils.h"

float getHeapUsedPercent() {
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    if (total == 0) return 0.0f;
    size_t freeBytes = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t used = total - freeBytes;
    return (float)used * 100.0f / (float)total;
}