#ifndef YGOENV_YGOPRO_YGO_DATA_TYPES_H_
#define YGOENV_YGOPRO_YGO_DATA_TYPES_H_

#include <cstdint>

namespace ygopro {

struct CardInfo {
  uint32_t code = 0;
  uint8_t controller = 0;
  uint8_t location = 0;
  uint8_t sequence = 0;
  uint8_t position = 0;
  uint32_t type = 0;
  uint32_t attack = 0;
  uint32_t defense = 0;
  uint32_t level = 0;
  uint32_t race = 0;
  uint32_t attribute = 0;
};

}  // namespace ygopro

#endif  // YGOENV_YGOPRO_YGO_DATA_TYPES_H_ 