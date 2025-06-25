#include "ygoenv/ygopro/ygo_core_adapter.h"
#include <cstring>
#include <stdexcept>
#include <random>

// Replicating the include block from ygopro.h
#include "ygopro-core/common.h"
#include "ygopro-core/card_data.h"
#include "ygopro-core/duel.h"
#include "ygopro-core/ocgapi.h"

#include "ygoenv/ygopro/ygo_data_types.h"

namespace ygopro {

// A helper class to read from the query buffer
class QueryBufferReader {
public:
    QueryBufferReader(const uint8_t* buf) : p_(buf) {}

    uint32_t read_u32() {
        uint32_t val;
        std::memcpy(&val, p_, sizeof(uint32_t));
        p_ += sizeof(uint32_t);
        return val;
    }

    uint8_t read_u8() {
        return *p_++;
    }

private:
    const uint8_t* p_;
};

YgoCoreAdapter::YgoCoreAdapter(uint32_t seed) {
  std::mt19937 rnd(seed);
  rnd(); // burn 1
  pduel_ = ::create_duel(rnd());
  if (!pduel_) {
    throw std::runtime_error("Failed to create duel.");
  }
}

YgoCoreAdapter::~YgoCoreAdapter() {
  if (pduel_) {
    ::end_duel(pduel_);
  }
}

void YgoCoreAdapter::set_player_info(int32_t playerid, int32_t lp, int32_t startcount, int32_t drawcount) {
  ::set_player_info(pduel_, playerid, lp, startcount, drawcount);
}

void YgoCoreAdapter::add_card(uint32_t cardcode, uint8_t owner, uint8_t playerid, uint8_t location, uint8_t sequence, uint8_t position) {
  ::new_card(pduel_, cardcode, owner, playerid, location, sequence, position);
}

void YgoCoreAdapter::start_duel(int32_t options) {
  ::start_duel(pduel_, options);
}

uint32_t YgoCoreAdapter::process() {
  return ::process(pduel_);
}

int32_t YgoCoreAdapter::get_message(uint8_t* buf) {
  return ::get_message(pduel_, buf);
}

void YgoCoreAdapter::set_response(int32_t value) {
  ::set_responsei(pduel_, value);
}

void YgoCoreAdapter::set_response(const std::vector<uint8_t>& buf) {
  std::vector<uint8_t> non_const_buf = buf;
  ::set_responseb(pduel_, non_const_buf.data());
}

std::vector<CardInfo> YgoCoreAdapter::query_field(uint8_t playerid) {
    uint8_t query_buffer[4096];
    int32_t len = ::query_field_card(pduel_, playerid, LOCATION_MZONE | LOCATION_SZONE | LOCATION_HAND, 0x781fff, query_buffer, 4096);
    if (len <= 0) {
        return {};
    }

    std::vector<CardInfo> cards;
    QueryBufferReader reader(query_buffer);
    int count = len / 32; // Each card is 32 bytes in this query
    for (int i = 0; i < count; ++i) {
        CardInfo card;
        uint32_t code = reader.read_u32();
        if (code == 0) {
            // Skip padding bytes
            for(int j = 1; j < 8; ++j) reader.read_u32();
            continue;
        }
        card.code = code;
        card.controller = reader.read_u8();
        card.location = reader.read_u8();
        card.sequence = reader.read_u8();
        card.position = reader.read_u8();
        card.type = reader.read_u32();
        card.attack = reader.read_u32();
        card.defense = reader.read_u32();
        card.level = reader.read_u32();
        card.race = reader.read_u32();
        card.attribute = reader.read_u32();
        cards.push_back(card);
    }
    return cards;
}

CardInfo YgoCoreAdapter::query_card(uint8_t playerid, uint8_t location, uint8_t sequence) {
    uint8_t query_buffer[1024];
    int32_t len = ::query_card(pduel_, playerid, location, sequence, 0x781fff, query_buffer, 1);
    if (len <= 0) {
        throw std::runtime_error("Failed to query card.");
    }

    QueryBufferReader reader(query_buffer);
    CardInfo card;
    card.code = reader.read_u32();
    reader.read_u32(); // alias
    card.type = reader.read_u32();
    card.level = reader.read_u32();
    card.race = reader.read_u32();
    card.attribute = reader.read_u32();
    card.attack = reader.read_u32();
    card.defense = reader.read_u32();
    // Skipping many other fields for simplicity as planned
    return card;
}

}  // namespace ygopro 