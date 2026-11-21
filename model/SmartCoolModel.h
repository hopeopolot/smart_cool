#ifndef SMARTCOOL_MODEL_H
#define SMARTCOOL_MODEL_H
#include <Arduino.h>

// Exported SmartCool Decision Tree. Feature order must match training:
// 0 Temperature Front (deg C), 1 Humidity Front (%),
// 2 Temperature Middle (deg C), 3 Humidity Middle (%),
// 4 Temperature Back (deg C), 5 Humidity Back (%).
// Returns 1 = predicted overheating within the next 3 samples; 0 = normal.

static const int8_t SC_LEFT[59] = {1, 2, 3, 4, 5, -1, -1, 8, -1, -1, 11, 12, -1, -1, 15, -1, -1, 18, 19, 20, -1, -1, 23, -1, -1, 26, 27, -1, -1, -1, 31, 32, 33, 34, -1, -1, 37, -1, -1, 40, 41, -1, -1, 44, -1, -1, 47, 48, 49, -1, -1, 52, -1, -1, 55, 56, -1, -1, -1};
static const int8_t SC_RIGHT[59] = {30, 17, 10, 7, 6, -1, -1, 9, -1, -1, 14, 13, -1, -1, 16, -1, -1, 25, 22, 21, -1, -1, 24, -1, -1, 29, 28, -1, -1, -1, 46, 39, 36, 35, -1, -1, 38, -1, -1, 43, 42, -1, -1, 45, -1, -1, 54, 51, 50, -1, -1, 53, -1, -1, 58, 57, -1, -1, -1};
static const int8_t SC_FEATURE[59] = {2, 0, 5, 5, 1, -2, -2, 2, -2, -2, 2, 0, -2, -2, 1, -2, -2, 2, 3, 0, -2, -2, 4, -2, -2, 1, 5, -2, -2, -2, 2, 1, 5, 0, -2, -2, 4, -2, -2, 0, 3, -2, -2, 3, -2, -2, 2, 3, 4, -2, -2, 1, -2, -2, 1, 2, -2, -2, -2};
static const float SC_THRESHOLD[59] = {29.5f, 26.5f, 39.5f, 33.5f, 34.5f, -2f, -2f, 24.5f, -2f, -2f, 28.5f, 24.5f, -2f, -2f, 67f, -2f, -2f, 28.5f, 65.5f, 27.5f, -2f, -2f, 25.5f, -2f, -2f, 70.5f, 55.5f, -2f, -2f, -2f, 30.5f, 46.5f, 48.5f, 28.5f, -2f, -2f, 28.5f, -2f, -2f, 30.5f, 61f, -2f, -2f, 57.5f, -2f, -2f, 31.5f, 56.5f, 29.5f, -2f, -2f, 54.5f, -2f, -2f, 66.5f, 32.5f, -2f, -2f, -2f};
static const uint8_t SC_CLASS[59] = {0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0};

inline int predictSmartCool(const float x[6]) {
  int node = 0;
  while (SC_FEATURE[node] != -2) {
    const int f = SC_FEATURE[node];
    node = (x[f] <= SC_THRESHOLD[node]) ? SC_LEFT[node] : SC_RIGHT[node];
    if (node < 0) return 0;
  }
  return (int)SC_CLASS[node];
}

#endif
