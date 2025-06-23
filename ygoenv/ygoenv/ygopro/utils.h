#ifndef YGOENV_YGOPRO_UTILS_H_
#define YGOENV_YGOPRO_UTILS_H_

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>
#include <ankerl/unordered_dense.h>

#include "ygopro-core/common.h"

namespace ygopro {

// Forward declarations
class Card;

// ============================================================================
// MATHEMATICAL/PARSING UTILITIES
// ============================================================================

// TODO: Move combinations, sum_to, parse_codes_from_opcodes functions here

inline std::vector<std::vector<int>> combinations(int n, int r) {
  std::vector<std::vector<int>> combs;
  std::vector<bool> m(n);
  std::fill(m.begin(), m.begin() + r, true);

  do {
    std::vector<int> cs;
    cs.reserve(r);
    for (int i = 0; i < n; ++i) {
      if (m[i]) {
        cs.push_back(i);
      }
    }
    combs.push_back(cs);
  } while (std::prev_permutation(m.begin(), m.end()));

  return combs;
}

inline bool sum_to(const std::vector<int> &w, const std::vector<int> ind, int i,
                   int r) {
  if (r <= 0) {
    return false;
  }
  int n = ind.size();
  if (i == n - 1) {
    return r == 1 || (w[ind[i]] == r);
  }
  return sum_to(w, ind, i + 1, r - 1) || sum_to(w, ind, i + 1, r - w[ind[i]]);
}

inline bool sum_to(const std::vector<int> &w, const std::vector<int> ind,
                   int r) {
  return sum_to(w, ind, 0, r);
}

inline std::vector<std::vector<int>>
combinations_with_weight(const std::vector<int> &weights, int r) {
  int n = weights.size();
  std::vector<std::vector<int>> results;

  for (int k = 1; k <= n; k++) {
    std::vector<std::vector<int>> combs = combinations(n, k);
    for (const auto &comb : combs) {
      if (sum_to(weights, comb, r)) {
        results.push_back(comb);
      }
    }
  }
  return results;
}

inline bool sum_to2(const std::vector<std::vector<int>> &w,
                    const std::vector<int> ind, int i, int r) {
  if (r <= 0) {
    return false;
  }
  int n = ind.size();
  const auto &w_ = w[ind[i]];
  if (i == n - 1) {
    if (w_.size() == 1) {
      return w_[0] == r;
    } else {
      return w_[0] == r || w_[1] == r;
    }
  }
  if (w_.size() == 1) {
    return sum_to2(w, ind, i + 1, r - w_[0]);
  } else {
    return sum_to2(w, ind, i + 1, r - w_[0]) ||
           sum_to2(w, ind, i + 1, r - w_[1]);
  }
}

inline bool sum_to2(const std::vector<std::vector<int>> &w,
                    const std::vector<int> ind, int r) {
  return sum_to2(w, ind, 0, r);
}

inline std::vector<std::vector<int>>
combinations_with_weight2(
  const std::vector<std::vector<int>> &weights, int r) {
  int n = weights.size();
  std::vector<std::vector<int>> results;

  for (int k = 1; k <= n; k++) {
    std::vector<std::vector<int>> combs = combinations(n, k);
    for (const auto &comb : combs) {
      if (sum_to2(weights, comb, r)) {
        results.push_back(comb);
      }
    }
  }
  return results;
}

inline std::vector<uint32_t>
parse_codes_from_opcodes(const std::vector<uint32_t> &opcodes) {
  int n = opcodes.size();
  std::vector<uint32_t> codes;

  if (n == 2) {
    codes.push_back(opcodes[0]);
    return codes;
  }

  if (((n - 2) % 3) != 0) {
    for (int i = 0; i < n; i++) {
      fmt::println("{}: {}", i, opcodes[i]);
    }
    throw std::runtime_error("invalid format of opcodes");
  }

  for (int i = 2; i < n; i += 3) {
    codes.push_back(opcodes[i]);
    if ((opcodes[i + 1] != 1073742080) || (opcodes[i + 2] != 1073741829)) {
      for (int i = 0; i < n; i++) {
        fmt::println("{}: {}", i, opcodes[i]);
      }
      auto err = fmt::format("invalid format of opcodes starting from {}", i);
      throw std::runtime_error(err);
    }
  }
  return codes;
}

// ============================================================================
// STRING MAPPING CONSTANTS 
// ============================================================================

// TODO: Move system_strings, position2str, phase2str, etc. here

static std::string msg_to_string(int msg) {
  switch (msg) {
  case MSG_RETRY:
    return "retry";
  case MSG_HINT:
    return "hint";
  case MSG_WIN:
    return "win";
  case MSG_SELECT_BATTLECMD:
    return "select_battlecmd";
  case MSG_SELECT_IDLECMD:
    return "select_idlecmd";
  case MSG_SELECT_EFFECTYN:
    return "select_effectyn";
  case MSG_SELECT_YESNO:
    return "select_yesno";
  case MSG_SELECT_OPTION:
    return "select_option";
  case MSG_SELECT_CARD:
    return "select_card";
  case MSG_SELECT_CHAIN:
    return "select_chain";
  case MSG_SELECT_PLACE:
    return "select_place";
  case MSG_SELECT_POSITION:
    return "select_position";
  case MSG_SELECT_TRIBUTE:
    return "select_tribute";
  case MSG_SELECT_COUNTER:
    return "select_counter";
  case MSG_SELECT_SUM:
    return "select_sum";
  case MSG_SELECT_DISFIELD:
    return "select_disfield";
  case MSG_SORT_CARD:
    return "sort_card";
  case MSG_SELECT_UNSELECT_CARD:
    return "select_unselect_card";
  case MSG_CONFIRM_DECKTOP:
    return "confirm_decktop";
  case MSG_CONFIRM_CARDS:
    return "confirm_cards";
  case MSG_SHUFFLE_DECK:
    return "shuffle_deck";
  case MSG_SHUFFLE_HAND:
    return "shuffle_hand";
  case MSG_SWAP_GRAVE_DECK:
    return "swap_grave_deck";
  case MSG_SHUFFLE_SET_CARD:
    return "shuffle_set_card";
  case MSG_REVERSE_DECK:
    return "reverse_deck";
  case MSG_DECK_TOP:
    return "deck_top";
  case MSG_SHUFFLE_EXTRA:
    return "shuffle_extra";
  case MSG_NEW_TURN:
    return "new_turn";
  case MSG_NEW_PHASE:
    return "new_phase";
  case MSG_CONFIRM_EXTRATOP:
    return "confirm_extratop";
  case MSG_MOVE:
    return "move";
  case MSG_POS_CHANGE:
    return "pos_change";
  case MSG_SET:
    return "set";
  case MSG_SWAP:
    return "swap";
  case MSG_FIELD_DISABLED:
    return "field_disabled";
  case MSG_SUMMONING:
    return "summoning";
  case MSG_SUMMONED:
    return "summoned";
  case MSG_SPSUMMONING:
    return "spsummoning";
  case MSG_SPSUMMONED:
    return "spsummoned";
  case MSG_FLIPSUMMONING:
    return "flipsummoning";
  case MSG_FLIPSUMMONED:
    return "flipsummoned";
  case MSG_CHAINING:
    return "chaining";
  case MSG_CHAINED:
    return "chained";
  case MSG_CHAIN_SOLVING:
    return "chain_solving";
  case MSG_CHAIN_SOLVED:
    return "chain_solved";
  case MSG_CHAIN_END:
    return "chain_end";
  case MSG_CHAIN_NEGATED:
    return "chain_negated";
  case MSG_CHAIN_DISABLED:
    return "chain_disabled";
  case MSG_RANDOM_SELECTED:
    return "random_selected";
  case MSG_BECOME_TARGET:
    return "become_target";
  case MSG_DRAW:
    return "draw";
  case MSG_DAMAGE:
    return "damage";
  case MSG_RECOVER:
    return "recover";
  case MSG_EQUIP:
    return "equip";
  case MSG_LPUPDATE:
    return "lpupdate";
  case MSG_CARD_TARGET:
    return "card_target";
  case MSG_CANCEL_TARGET:
    return "cancel_target";
  case MSG_PAY_LPCOST:
    return "pay_lpcost";
  case MSG_ADD_COUNTER:
    return "add_counter";
  case MSG_REMOVE_COUNTER:
    return "remove_counter";
  case MSG_ATTACK:
    return "attack";
  case MSG_BATTLE:
    return "battle";
  case MSG_ATTACK_DISABLED:
    return "attack_disabled";
  case MSG_DAMAGE_STEP_START:
    return "damage_step_start";
  case MSG_DAMAGE_STEP_END:
    return "damage_step_end";
  case MSG_MISSED_EFFECT:
    return "missed_effect";
  case MSG_TOSS_COIN:
    return "toss_coin";
  case MSG_TOSS_DICE:
    return "toss_dice";
  case MSG_ROCK_PAPER_SCISSORS:
    return "rock_paper_scissors";
  case MSG_HAND_RES:
    return "hand_res";
  case MSG_ANNOUNCE_RACE:
    return "announce_race";
  case MSG_ANNOUNCE_ATTRIB:
    return "announce_attrib";
  case MSG_ANNOUNCE_CARD:
    return "announce_card";
  case MSG_ANNOUNCE_NUMBER:
    return "announce_number";
  case MSG_CARD_HINT:
    return "card_hint";
  case MSG_TAG_SWAP:
    return "tag_swap";
  case MSG_RELOAD_FIELD:
    return "reload_field";
  case MSG_AI_NAME:
    return "ai_name";
  case MSG_SHOW_HINT:
    return "show_hint";
  case MSG_PLAYER_HINT:
    return "player_hint";
  case MSG_MATCH_KILL:
    return "match_kill";
  case MSG_CUSTOM_MSG:
    return "custom_msg";
  default:
    return "unknown_msg";
  }
}

// system string
static const std::map<int, std::string> system_strings = {
    // announce type
    {1050, "Monster"},
    {1051, "Spell"},
    {1052, "Trap"},
    {1054, "Normal"},
    {1055, "Effect"},
    {1056, "Fusion"},
    {1057, "Ritual"},
    {1058, "Trap Monsters"},
    {1059, "Spirit"},
    {1060, "Union"},
    {1061, "Gemini"},
    {1062, "Tuner"},
    {1063, "Synchro"},
    {1064, "Token"},
    {1066, "Quick-Play"},
    {1067, "Continuous"},
    {1068, "Equip"},
    {1069, "Field"},
    {1070, "Counter"},
    {1071, "Flip"},
    {1072, "Toon"},
    {1073, "Xyz"},
    {1074, "Pendulum"},
    {1075, "Special Summon"},
    {1076, "Link"},
    {1080, "(N/A)"},
    {1081, "Extra Monster Zone"},
    // announce type end
    // actions
    {1150, "Activate"},
    {1151, "Normal Summon"},
    {1152, "Special Summon"},
    {1153, "Set"},
    {1154, "Flip Summon"},
    {1155, "To Defense"},
    {1156, "To Attack"},
    {1157, "Attack"},
    {1158, "View"},
    {1159, "S/T Set"},
    {1160, "Put in Pendulum Zone"},
    {1161, "Do Effect"},
    {1162, "Reset Effect"},
    {1163, "Pendulum Summon"},
    {1164, "Synchro Summon"},
    {1165, "Xyz Summon"},
    {1166, "Link Summon"},
    {1167, "Tribute Summon"},
    {1168, "Ritual Summon"},
    {1169, "Fusion Summon"},
    {1190, "Add to hand"},
    {1191, "Send to GY"},
    {1192, "Banish"},
    {1193, "Return to Deck"},
    // actions end
    {1, "Normal Summon"},
    {30, "Replay rules apply. Continue this attack?"},
    {31, "Attack directly with this monster?"},
    {80, "Start Step of the Battle Phase."},
    {81, "During the End Phase."},
    {90, "Conduct this Normal Summon without Tributing?"},
    {91, "Use additional Summon?"},
    {92, "Tribute your opponent's monster?"},
    {93, "Continue selecting Materials?"},
    {94, "Activate this card's effect now?"},
    {95, "Use the effect of [%ls]?"},
    {96, "Use the effect of [%ls] to avoid destruction?"},
    {97, "Place [%ls] to a Spell & Trap Zone?"},
    {98, "Tribute a monster(s) your opponent controls?"},
    {200, "From [%ls], activate [%ls]?"},
    {203, "Chain another card or effect?"},
    {210, "Continue selecting?"},
    {218, "Pay LP by Effect of [%ls], instead?"},
    {219, "Detach Xyz material by Effect of [%ls], instead?"},
    {220, "Remove Counter(s) by Effect of [%ls], instead?"},
    {221, "On [%ls], Activate Trigger Effect of [%ls]?"},
    {222, "Activate Trigger Effect?"},
    {221, "On [%ls], Activate Trigger Effect of [%ls]?"},
    {1621, "Attack Negated"},
    {1622, "[%ls] Missed timing"}
};

#define POS_NONE 0x0 // xyz materials (overlay) ???

static const std::map<uint8_t, std::string> position2str = {
    {POS_NONE, "none"},
    {POS_FACEUP_ATTACK, "face-up attack"},
    {POS_FACEDOWN_ATTACK, "face-down attack"},
    {POS_ATTACK, "attack"},
    {POS_FACEUP_DEFENSE, "face-up defense"},
    {POS_FACEUP, "face-up"},
    {POS_FACEDOWN_DEFENSE, "face-down defense"},
    {POS_FACEDOWN, "face-down"},
    {POS_DEFENSE, "defense"},
};

#define ATTRIBUTE_NONE 0x0 // token

static const std::map<uint8_t, std::string> attribute2str = {
    {ATTRIBUTE_NONE, "None"},   {ATTRIBUTE_EARTH, "Earth"},
    {ATTRIBUTE_WATER, "Water"}, {ATTRIBUTE_FIRE, "Fire"},
    {ATTRIBUTE_WIND, "Wind"},   {ATTRIBUTE_LIGHT, "Light"},
    {ATTRIBUTE_DARK, "Dark"},   {ATTRIBUTE_DEVINE, "Divine"},
};

#define RACE_NONE 0x0 // token

static const std::map<uint32_t, std::string> race2str = {
    {RACE_NONE, "None"},
    {RACE_WARRIOR, "Warrior"},
    {RACE_SPELLCASTER, "Spellcaster"},
    {RACE_FAIRY, "Fairy"},
    {RACE_FIEND, "Fiend"},
    {RACE_ZOMBIE, "Zombie"},
    {RACE_MACHINE, "Machine"},
    {RACE_AQUA, "Aqua"},
    {RACE_PYRO, "Pyro"},
    {RACE_ROCK, "Rock"},
    {RACE_WINDBEAST, "Windbeast"},
    {RACE_PLANT, "Plant"},
    {RACE_INSECT, "Insect"},
    {RACE_THUNDER, "Thunder"},
    {RACE_DRAGON, "Dragon"},
    {RACE_BEAST, "Beast"},
    {RACE_BEASTWARRIOR, "Beast Warrior"},
    {RACE_DINOSAUR, "Dinosaur"},
    {RACE_FISH, "Fish"},
    {RACE_SEASERPENT, "Sea Serpent"},
    {RACE_REPTILE, "Reptile"},
    {RACE_PSYCHO, "Psycho"},
    {RACE_DEVINE, "Divine"},
    {RACE_CREATORGOD, "Creator God"},
    {RACE_WYRM, "Wyrm"},
    {RACE_CYBERSE, "Cyberse"},
    {RACE_ILLUSION, "Illusion"}};

static const std::map<uint32_t, std::string> type2str = {
    {TYPE_MONSTER, "Monster"},
    {TYPE_SPELL, "Spell"},
    {TYPE_TRAP, "Trap"},
    {TYPE_NORMAL, "Normal"},
    {TYPE_EFFECT, "Effect"},
    {TYPE_FUSION, "Fusion"},
    {TYPE_RITUAL, "Ritual"},
    {TYPE_TRAPMONSTER, "Trap Monster"},
    {TYPE_SPIRIT, "Spirit"},
    {TYPE_UNION, "Union"},
    {TYPE_DUAL, "Dual"},
    {TYPE_TUNER, "Tuner"},
    {TYPE_SYNCHRO, "Synchro"},
    {TYPE_TOKEN, "Token"},
    {TYPE_QUICKPLAY, "Quick-play"},
    {TYPE_CONTINUOUS, "Continuous"},
    {TYPE_EQUIP, "Equip"},
    {TYPE_FIELD, "Field"},
    {TYPE_COUNTER, "Counter"},
    {TYPE_FLIP, "Flip"},
    {TYPE_TOON, "Toon"},
    {TYPE_XYZ, "XYZ"},
    {TYPE_PENDULUM, "Pendulum"},
    {TYPE_SPSUMMON, "Special"},
    {TYPE_LINK, "Link"},
};

static const std::map<int, std::string> phase2str = {
    {PHASE_DRAW, "draw phase"},
    {PHASE_STANDBY, "standby phase"},
    {PHASE_MAIN1, "main1 phase"},
    {PHASE_BATTLE_START, "battle start phase"},
    {PHASE_BATTLE_STEP, "battle step phase"},
    {PHASE_DAMAGE, "damage phase"},
    {PHASE_DAMAGE_CAL, "damage calculation phase"},
    {PHASE_BATTLE, "battle phase"},
    {PHASE_MAIN2, "main2 phase"},
    {PHASE_END, "end phase"},
};

static const std::map<uint8_t, std::string> location2str = {
    {LOCATION_DECK, "Deck"},
    {LOCATION_HAND, "Hand"},
    {LOCATION_MZONE, "Main Monster Zone"},
    {LOCATION_SZONE, "Spell & Trap Zone"},
    {LOCATION_GRAVE, "Graveyard"},
    {LOCATION_REMOVED, "Banished"},
    {LOCATION_EXTRA, "Extra Deck"},
};

// ============================================================================
// STRING CONVERSION MACROS
// ============================================================================

#define DEFINE_X_TO_STRING_FUN(name, x_map) \
inline std::string name(decltype(x_map)::key_type x) { \
  auto it = x_map.find(x); \
  if (it != x_map.end()) { \
    return it->second; \
  } \
  return "unknown"; \
}

DEFINE_X_TO_STRING_FUN(attribute_to_string, attribute2str)
DEFINE_X_TO_STRING_FUN(phase_to_string, phase2str)
DEFINE_X_TO_STRING_FUN(position_to_string, position2str)

// ============================================================================
// ID TEMPLATE FUNCTIONS
// ============================================================================

template <class K = uint8_t>
ankerl::unordered_dense::map<K, uint8_t>
make_ids(const std::map<K, std::string> &m, int id_offset = 0,
         int m_offset = 0) {
  ankerl::unordered_dense::map<K, uint8_t> m2;
  int i = 0;
  for (const auto &[k, v] : m) {
    if (i < m_offset) {
      i++;
      continue;
    }
    m2[k] = i - m_offset + id_offset;
    i++;
  }
  return m2;
}

template <class K = char>
ankerl::unordered_dense::map<K, uint8_t>
make_ids(const std::vector<K> &cmds, int id_offset = 0, int m_offset = 0) {
  ankerl::unordered_dense::map<K, uint8_t> m2;
  for (int i = m_offset; i < cmds.size(); i++) {
    m2[cmds[i]] = i - m_offset + id_offset;
  }
  return m2;
}

// ============================================================================
// ID CONVERSION MACROS
// ============================================================================

#define DEFINE_X_TO_ID_FUN(name, x_map) \
inline uint8_t name(decltype(x_map)::key_type x) { \
  auto it = x_map.find(x); \
  if (it != x_map.end()) { \
    return it->second; \
  } \
  throw std::runtime_error( \
    fmt::format("[" #name "] cannot find id: {}", x)); \
}

// ============================================================================
// ID CONVERSION FUNCTIONS
// ============================================================================

static const ankerl::unordered_dense::map<int, uint8_t> system_string2id =
    make_ids(system_strings, 16);
DEFINE_X_TO_ID_FUN(system_string_to_id, system_string2id)


static const ankerl::unordered_dense::map<uint8_t, uint8_t> location2id =
    make_ids(location2str, 1);
DEFINE_X_TO_ID_FUN(location_to_id, location2id)


static const ankerl::unordered_dense::map<uint8_t, uint8_t> position2id =
    make_ids(position2str);
DEFINE_X_TO_ID_FUN(position_to_id, position2id)


static const ankerl::unordered_dense::map<uint8_t, uint8_t> attribute2id =
    make_ids(attribute2str);
DEFINE_X_TO_ID_FUN(attribute_to_id, attribute2id)

static const ankerl::unordered_dense::map<uint32_t, uint8_t> race2id =
    make_ids(race2str);
DEFINE_X_TO_ID_FUN(race_to_id, race2id)

inline std::vector<uint8_t> type_to_ids(uint32_t type) {
  std::vector<uint8_t> ids;
  ids.reserve(type2str.size());
  for (const auto &[k, v] : type2str) {
    ids.push_back(std::min(1u, type & k));
  }
  return ids;
}

static const ankerl::unordered_dense::map<int, uint8_t> phase2id =
    make_ids(phase2str);
DEFINE_X_TO_ID_FUN(phase_to_id, phase2id)


static const std::vector<int> _msgs = {
    MSG_SELECT_IDLECMD,  MSG_SELECT_CHAIN,     MSG_SELECT_CARD,
    MSG_SELECT_TRIBUTE,  MSG_SELECT_POSITION,  MSG_SELECT_EFFECTYN,
    MSG_SELECT_YESNO,    MSG_SELECT_BATTLECMD, MSG_SELECT_UNSELECT_CARD,
    MSG_SELECT_OPTION,   MSG_SELECT_PLACE,     MSG_SELECT_SUM,
    MSG_SELECT_DISFIELD, MSG_ANNOUNCE_ATTRIB,  MSG_ANNOUNCE_NUMBER,
    MSG_ANNOUNCE_CARD,
};

static const ankerl::unordered_dense::map<int, uint8_t> msg2id =
    make_ids(_msgs, 1);
DEFINE_X_TO_ID_FUN(msg_to_id, msg2id)


// ============================================================================
// GENERAL UTILITIES
// ============================================================================

static std::string ltrim(std::string s) {
  s.erase(s.begin(),
          std::find_if(s.begin(), s.end(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

inline std::string time_now() {
  // strftime %Y-%m-%d %H-%M-%S
  time_t now = time(0);
  tm *ltm = localtime(&now);
  char buffer[80];
  strftime(buffer, 80, "%Y-%m-%d %H-%M-%S", ltm);
  return std::string(buffer);
}

static std::vector<int> find_substrs(const std::string &str,
                                     const std::string &substr) {
  std::vector<int> res;
  int pos = 0;
  while ((pos = str.find(substr, pos)) != std::string::npos) {
    res.push_back(pos);
    pos += substr.length();
  }
  return res;
}

// ============================================================================
// GAME/MESSAGE UTILITIES  
// ============================================================================

static std::string get_system_string(int desc) {
  auto it = system_strings.find(desc);
  if (it != system_strings.end()) {
    return it->second;
  }
  throw std::runtime_error(
      fmt::format("Cannot find system string: {}", desc));
  // return "system string " + std::to_string(desc);
}

static std::string reason_to_string(uint8_t reason) {
  // !victory 0x0 Surrendered
  // !victory 0x1 LP reached 0
  // !victory 0x2 Cards can't be drawn
  // !victory 0x3 Time limit up
  // !victory 0x4 Lost connection
  switch (reason) {
  case 0x0:
    return "Surrendered";
  case 0x1:
    return "LP reached 0";
  case 0x2:
    return "Cards can't be drawn";
  case 0x3:
    return "Time limit up";
  case 0x4:
    return "Lost connection";
  default:
    return "Unknown";
  }
}

// ============================================================================
// CARD/LOCATION UTILITIES
// ============================================================================

inline std::string ls_to_spec(uint8_t loc, uint8_t seq, uint8_t pos) {
  std::string spec;
  if (loc & LOCATION_HAND) {
    spec += "h";
  } else if (loc & LOCATION_MZONE) {
    spec += "m";
  } else if (loc & LOCATION_SZONE) {
    spec += "s";
  } else if (loc & LOCATION_GRAVE) {
    spec += "g";
  } else if (loc & LOCATION_REMOVED) {
    spec += "r";
  } else if (loc & LOCATION_EXTRA) {
    spec += "x";
  }
  spec += std::to_string(seq + 1);
  if (loc & LOCATION_OVERLAY) {
    spec.push_back('a' + pos);
  }
  return spec;
}

inline std::string ls_to_spec(uint8_t loc, uint8_t seq, uint8_t pos, bool opponent) {
  std::string spec = ls_to_spec(loc, seq, pos);
  if (opponent) {
    spec.insert(0, 1, 'o');
  }
  return spec;
}

inline std::tuple<uint8_t, uint8_t, uint8_t>
spec_to_ls(const std::string spec) {
  uint8_t loc;
  uint8_t seq;
  uint8_t pos = 0;
  int offset = 1;
  if (spec[0] == 'h') {
    loc = LOCATION_HAND;
  } else if (spec[0] == 'm') {
    loc = LOCATION_MZONE;
  } else if (spec[0] == 's') {
    loc = LOCATION_SZONE;
  } else if (spec[0] == 'g') {
    loc = LOCATION_GRAVE;
  } else if (spec[0] == 'r') {
    loc = LOCATION_REMOVED;
  } else if (spec[0] == 'x') {
    loc = LOCATION_EXTRA;
  } else if (std::isdigit(spec[0])) {
    loc = LOCATION_DECK;
    offset = 0;
  } else {
    std::string s = fmt::format("Invalid spec {}", spec);
    throw std::runtime_error(s);
  }
  int end = offset;
  while (end < spec.size() && std::isdigit(spec[end])) {
    end++;
  }
  seq = std::stoi(spec.substr(offset, end - offset)) - 1;
  if (end < spec.size()) {
    pos = spec[end] - 'a';
  }
  return {loc, seq, pos};
}


inline std::tuple<uint8_t, uint8_t, uint8_t, uint8_t>
spec_to_ls(uint8_t player, const std::string spec) {
  uint8_t controller = player;
  int offset = 0;
  if (spec[0] == 'o') {
    controller = 1 - player;
    offset++;
  }
  auto [loc, seq, pos] = spec_to_ls(spec.substr(offset));
  return {controller, loc, seq, pos};
}

// ============================================================================
// DECK UTILITIES
// ============================================================================

static std::tuple<std::vector<uint32>, std::vector<uint32>, std::vector<uint32>> read_decks(const std::string &fp) {
  std::ifstream file(fp);
  std::string line;
  std::vector<uint32> main_deck, extra_deck, side_deck;
  bool found_extra = false;

  if (file.is_open()) {
    // Read the main deck
    while (std::getline(file, line)) {
      if (line.find("side") != std::string::npos) {
        break;
      }
      if (line.find("extra") != std::string::npos) {
        found_extra = true;
        break;
      }
      // Check if line contains only digits
      if (std::all_of(line.begin(), line.end(), ::isdigit)) {
        main_deck.push_back(std::stoul(line));
      }
    }

    if (main_deck.size() < 40) {
      std::string err = fmt::format("Main deck must contain at least 40 cards, found: {}, file: {}", main_deck.size(), fp);
      throw std::runtime_error(err);
    }

    // Read the extra deck
    if (found_extra) {
      while (std::getline(file, line)) {
        if (line.find("side") != std::string::npos) {
          break;
        }
        // Check if line contains only digits
        if (std::all_of(line.begin(), line.end(), ::isdigit)) {
          extra_deck.push_back(std::stoul(line));
        }
      }
    }

    // Read the side deck
    while (std::getline(file, line)) {
      // Check if line contains only digits
      if (std::all_of(line.begin(), line.end(), ::isdigit)) {
        side_deck.push_back(std::stoul(line));
      }
    }

    file.close();
  } else {
    throw std::runtime_error(fmt::format("Unable to open deck file: {}", fp));
  }

  return std::make_tuple(main_deck, extra_deck, side_deck);
}


}  // namespace ygopro

#endif  // YGOENV_YGOPRO_UTILS_H_ 