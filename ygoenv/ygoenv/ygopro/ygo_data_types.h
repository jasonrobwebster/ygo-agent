#ifndef YGOENV_YGOPRO_YGO_DATA_TYPES_H_
#define YGOENV_YGOPRO_YGO_DATA_TYPES_H_

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <fmt/format.h>

#include "ygoenv/ygopro/utils.h"

namespace ygopro {

using PlayerId = uint8_t;
using CardCode = uint32_t;
using CardId = uint16_t;

const int DESCRIPTION_LIMIT = 10000;
const int CARD_EFFECT_OFFSET = 10010;

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

enum class ActionAct {
  None,
  Set,
  Repo,
  SpSummon,
  Summon,
  MSet,
  Attack,
  DirectAttack,
  Activate,
  Cancel,
};

inline std::string action_act_to_string(ActionAct act) {
  switch (act) {
  case ActionAct::None:
    return "None";
  case ActionAct::Set:
    return "Set";
  case ActionAct::Repo:
    return "Repo";
  case ActionAct::SpSummon:
    return "SpSummon";
  case ActionAct::Summon:
    return "Summon";
  case ActionAct::MSet:
    return "MSet";
  case ActionAct::Attack:
    return "Attack";
  case ActionAct::DirectAttack:
    return "DirectAttack";
  case ActionAct::Activate:
    return "Activate";
  case ActionAct::Cancel:
    return "Cancel";
  default:
    return "Unknown";
  }
}

enum class ActionPhase {
  None,
  Battle,
  Main2,
  End,
};

inline std::string action_phase_to_string(ActionPhase phase) {
  switch (phase) {
  case ActionPhase::None:
    return "None";
  case ActionPhase::Battle:
    return "Battle";
  case ActionPhase::Main2:
    return "Main2";
  case ActionPhase::End:
    return "End";
  default:
    return "Unknown";
  }
}

enum class ActionPlace {
  None,
  MZone1,
  MZone2,
  MZone3,
  MZone4,
  MZone5,
  MZone6,
  MZone7,
  SZone1,
  SZone2,
  SZone3,
  SZone4,
  SZone5,
  SZone6,
  SZone7,
  SZone8,
  OpMZone1,
  OpMZone2,
  OpMZone3,
  OpMZone4,
  OpMZone5,
  OpMZone6,
  OpMZone7,
  OpSZone1,
  OpSZone2,
  OpSZone3,
  OpSZone4,
  OpSZone5,
  OpSZone6,
  OpSZone7,
  OpSZone8,
};

inline std::vector<ActionPlace> flag_to_usable_places(
  uint32_t flag, bool reverse = false) {
  std::vector<ActionPlace> places;
  for (int j = 0; j < 4; j++) {
    uint32_t value = (flag >> (j * 8)) & 0xff;
    for (int i = 0; i < 8; i++) {
      bool avail = (value & (1 << i)) == 0;
      if (reverse) {
        avail = !avail;
      }
      if (avail) {
        ActionPlace place;
        if (j == 0) {
          place = static_cast<ActionPlace>(i + static_cast<int>(ActionPlace::MZone1));
        } else if (j == 1) {
          place = static_cast<ActionPlace>(i + static_cast<int>(ActionPlace::SZone1));
        } else if (j == 2) {
          place = static_cast<ActionPlace>(i + static_cast<int>(ActionPlace::OpMZone1));
        } else if (j == 3) {
          place = static_cast<ActionPlace>(i + static_cast<int>(ActionPlace::OpSZone1));
        }
        places.push_back(place);
      }
    }
  }
  return places;
}

inline std::string action_place_to_string(ActionPlace place) {
  int i = static_cast<int>(place);
  if (i == 0) {
    return "None";
  }
  else if (i >= static_cast<int>(ActionPlace::MZone1) && i <= static_cast<int>(ActionPlace::MZone7)) {
    return fmt::format("m{}", i - static_cast<int>(ActionPlace::MZone1) + 1);
  }
  else if (i >= static_cast<int>(ActionPlace::SZone1) && i <= static_cast<int>(ActionPlace::SZone8)) {
    return fmt::format("s{}", i - static_cast<int>(ActionPlace::SZone1) + 1);
  }
  else if (i >= static_cast<int>(ActionPlace::OpMZone1) && i <= static_cast<int>(ActionPlace::OpMZone7)) {
    return fmt::format("om{}", i - static_cast<int>(ActionPlace::OpMZone1) + 1);
  }
  else if (i >= static_cast<int>(ActionPlace::OpSZone1) && i <= static_cast<int>(ActionPlace::OpSZone8)) {
    return fmt::format("os{}", i - static_cast<int>(ActionPlace::OpSZone1) + 1);
  }
  else {
    return "Unknown";
  }
}

class Card {
  friend class YGOProEnvImpl;

protected:
  CardCode code_ = 0;
  uint32_t alias_;
  uint64_t setcode_;
  uint32_t type_;
  uint32_t level_;
  uint32_t lscale_;
  uint32_t rscale_;
  int32_t attack_;
  int32_t defense_;
  uint32_t race_;
  uint32_t attribute_;
  uint32_t link_marker_;
  std::string name_;
  std::string desc_;
  std::vector<std::string> strings_;

  uint32_t data_ = 0;

  uint32_t status_ = 0;
  PlayerId controler_ = 0;
  uint32_t location_ = 0;
  uint32_t sequence_ = 0;
  uint32_t position_ = 0;
  uint32_t counter_ = 0;

public:
  Card() = default;

  Card(CardCode code, uint32_t alias, uint64_t setcode, uint32_t type,
       uint32_t level, uint32_t lscale, uint32_t rscale, int32_t attack,
       int32_t defense, uint32_t race, uint32_t attribute, uint32_t link_marker,
       const std::string &name, const std::string &desc,
       const std::vector<std::string> &strings)
      : code_(code), alias_(alias), setcode_(setcode), type_(type),
        level_(level), lscale_(lscale), rscale_(rscale), attack_(attack),
        defense_(defense), race_(race), attribute_(attribute),
        link_marker_(link_marker), name_(name), desc_(desc), strings_(strings) {
  }

  ~Card() = default;

  void set_location(uint32_t location) {
    controler_ = location & 0xff;
    location_ = (location >> 8) & 0xff;
    sequence_ = (location >> 16) & 0xff;
    position_ = (location >> 24) & 0xff;
  }

  const std::string &name() const { return name_; }
  const std::string &desc() const { return desc_; }
  const uint32_t &type() const { return type_; }
  const uint32_t &level() const { return level_; }
  const std::vector<std::string> &strings() const { return strings_; }

  std::string get_spec(bool opponent) const {
    return ls_to_spec(location_, sequence_, position_, opponent);
  }

  std::string get_spec(PlayerId player) const {
    return get_spec(player != controler_);
  }

  std::string get_position() const {
    return position_to_string(position_);
  }

  std::string get_effect_description(CardCode code, int effect_idx) const {
    if (code == 0) {
      return get_system_string(effect_idx);
    }
    if (effect_idx == 0) {
      return "default";
    }
    effect_idx -= CARD_EFFECT_OFFSET;
    if (effect_idx < 0) {
      throw std::runtime_error("Invalid effect index: " + std::to_string(effect_idx));
    }
    auto s = strings_[effect_idx];
    if (s.empty()) {
      return "effect " + std::to_string(effect_idx);
    }
    return s;
  }
};

class LegalAction {
public:
  std::string spec_ = "";
  ActionAct act_ = ActionAct::None;
  ActionPhase phase_ = ActionPhase::None;
  bool finish_ = false;
  uint8_t position_ = 0;
  int effect_ = -1;
  uint8_t number_ = 0;
  ActionPlace place_ = ActionPlace::None;
  uint8_t attribute_ = 0;

  int spec_index_ = 0;
  CardId cid_ = 0;
  int msg_ = 0;
  uint32_t response_ = 0;

  static LegalAction from_spec(const std::string &spec) {
    LegalAction la;
    la.spec_ = spec;
    return la;
  }

  static LegalAction act_spec(ActionAct act, const std::string &spec) {
    LegalAction la;
    la.act_ = act;
    la.spec_ = spec;
    return la;
  }

  static LegalAction finish() {
    LegalAction la;
    la.finish_ = true;
    return la;
  }

  static LegalAction cancel() {
    LegalAction la;
    la.act_ = ActionAct::Cancel;
    return la;
  }

  static LegalAction activate_spec(int effect_idx, const std::string &spec) {
    LegalAction la;
    la.act_ = ActionAct::Activate;
    la.effect_ = effect_idx;
    la.spec_ = spec;
    return la;
  }

  static LegalAction phase(ActionPhase phase) {
    LegalAction la;
    la.phase_ = phase;
    return la;
  }

  static LegalAction number(uint8_t number) {
    LegalAction la;
    la.number_ = number;
    return la;
  }

  static LegalAction place(ActionPlace place) {
    LegalAction la;
    la.place_ = place;
    return la;
  }

  static LegalAction attribute(int attribute) {
    LegalAction la;
    la.attribute_ = attribute;
    return la;
  }
};

}  // namespace ygopro

#endif  // YGOENV_YGOPRO_YGO_DATA_TYPES_H_ 