#ifndef YGOENV_YGOPRO_YGO_CORE_ADAPTER_H_
#define YGOENV_YGOPRO_YGO_CORE_ADAPTER_H_
#include <cstdint>
#include <vector>

#include "ygoenv/ygopro/ygo_data_types.h"

namespace ygopro {

class YgoCoreAdapter {
 public:
  explicit YgoCoreAdapter(uint32_t seed);
  ~YgoCoreAdapter();

  void set_player_info(int32_t playerid, int32_t lp, int32_t startcount, int32_t drawcount);
  void add_card(uint32_t cardcode, uint8_t owner, uint8_t playerid, uint8_t location, uint8_t sequence, uint8_t position);
  void start_duel(int32_t options);

  uint32_t process();
  int32_t get_message(uint8_t* buf);
  void set_response(int32_t value);
  void set_response(const std::vector<uint8_t>& buf);

  std::vector<CardInfo> query_field(uint8_t playerid);
  CardInfo query_card(uint8_t playerid, uint8_t location, uint8_t sequence);

 private:
  // This is now an owning pointer.
  intptr_t pduel_ = 0;
};

}  // namespace ygopro

#endif  // YGOENV_YGOPRO_YGO_CORE_ADAPTER_H_ 