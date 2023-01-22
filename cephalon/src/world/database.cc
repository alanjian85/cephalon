#include "database.h"
using namespace cephalon;

#include "world.h"

#include <iostream>

#include <spdlog/spdlog.h>

int Database::loadIdCallback(void* self, int, char** texts, char**) {
    auto ptr = static_cast<Database*>(self);
    ptr->blocks_id_[texts[0]] = std::atoi(texts[1]);    
    return 0; 
}

Database::Database(const char* path) {
    int rc;
    rc = sqlite3_open(path, &db_);
    if (rc) {
        spdlog::error("Couldn't open database {}", path);
        return;
    } else {
        spdlog::info("Database {} is opened", path);
    }
    const char* create_query =
        "CREATE TABLE IF NOT EXISTS blocks ("
        "rx INT NOT NULL,"
        "ry INT NOT NULL,"
        "ox INT NOT NULL,"
        "oy INT NOT NULL,"
        "oz INT NOT NULL,"
        "name TEXT NOT NULL"
        ");"
        "CREATE UNIQUE INDEX IF NOT EXISTS blocks_position ON blocks (rx, ry, ox, oy, oz);"
        "CREATE TABLE IF NOT EXISTS blocks_id ("
        "id   INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL);";
    const char* insert_id_query = "INSERT INTO blocks_id (name) VALUES (?);";
    const char* load_id_query = "SELECT id, name FROM blocks_id;";
    const char* insert_query = "REPLACE INTO blocks (rx, ry, ox, oy, oz, name) VALUES (?, ?, ?, ?, ?, ?);";
    const char* load_query = "SELECT ox, oy, oz, name FROM blocks WHERE rx = ? AND ry = ?;";
    rc = sqlite3_exec(db_, create_query, nullptr, nullptr, nullptr);
    if (rc) spdlog::error("Failed to execute table create query");
    else spdlog::info("Table created successfully");
    rc = sqlite3_prepare(db_, insert_id_query, -1, &insert_id_stmt_, nullptr);
    if (rc) spdlog::error("Failed to prepare block ID insert statement");
    else spdlog::info("Block ID insert statement compiled successfully");
    rc = sqlite3_exec(db_, load_id_query, loadIdCallback, this, nullptr);
    if (rc) spdlog::error("Failed to load blocks' ID");
    else spdlog::info("Blocks' ID loaded successfully");
    rc = sqlite3_prepare(db_, insert_query, -1, &insert_stmt_, nullptr);
    if (rc) spdlog::error("Failed to prepare block insert statement");
    else spdlog::info("Block insert statement compiled successfully");
    rc = sqlite3_prepare(db_, load_query, -1, &load_stmt_, nullptr);
    if (rc) spdlog::error("Failed to prepare chunk load statement");
    else spdlog::info("Chunk load statement compiled successfully");
}

Database::~Database() {
    int rc;
    rc = sqlite3_finalize(load_stmt_);
    if (rc) spdlog::critical("Failed to finalize chunk load statement");
    else spdlog::info("Chunk load statement finalized");
    rc = sqlite3_finalize(insert_stmt_);
    if (rc) spdlog::critical("Failed to finalize block insert statement");
    else spdlog::info("Block insert statement finalized");
    rc = sqlite3_finalize(insert_id_stmt_); 
    if (rc) spdlog::critical("Failed to finalize block ID insert statement");
    else spdlog::info("Block ID insert statement finalized");
    rc = sqlite3_close(db_);
    if (rc) spdlog::critical("Failed to close database");
    else spdlog::info("Database closed");
}

void Database::loadChunk(Chunk& chunk) const {
    auto region = chunk.getRegion();
    std::lock_guard lock(load_mutex_);
    sqlite3_reset(load_stmt_);
    sqlite3_bind_int(load_stmt_, 1, region.x);
    sqlite3_bind_int(load_stmt_, 2, region.y);
    while (sqlite3_step(load_stmt_) == SQLITE_ROW) {
        auto ox = sqlite3_column_int(load_stmt_, 0);
        auto oy = sqlite3_column_int(load_stmt_, 1);
        auto oz = sqlite3_column_int(load_stmt_, 2);
       
 glm::ivec3 offset(ox, oy, oz);
        auto name = reinterpret_cast<const char*>(sqlite3_column_text(load_stmt_, 3));
        auto block = Block::getBlock(name);
        if (!block) {
            spdlog::warn("Unknown block name {}", name);
            continue;
        }
        chunk.setBlock(offset, *block);
    }
}

void Database::insertBlock(glm::ivec3 pos, const char* name) {
    auto region = World::getRegion(pos);
    auto offset = World::getOffset(pos);
    std::lock_guard lock(insert_mutex_);
    sqlite3_reset(insert_stmt_);
    sqlite3_bind_int(insert_stmt_, 1, region.x);
    sqlite3_bind_int(insert_stmt_, 2, region.y);
    sqlite3_bind_int(insert_stmt_, 3, offset.x);
    sqlite3_bind_int(insert_stmt_, 4, offset.y);
    sqlite3_bind_int(insert_stmt_, 5, offset.z);
    sqlite3_bind_text(insert_stmt_, 6, name, -1, nullptr);
    int rc = sqlite3_step(insert_stmt_);
    if (rc != SQLITE_DONE) spdlog::critical("Failed to execute block insert statement");
    else spdlog::debug("Block inserted successfully");
}

int Database::getBlockIndex(const std::string& name) {
    auto it = blocks_id_.find(name);
    if (it != blocks_id_.cend())
        return it->second;
    sqlite3_bind_text(insert_id_stmt_, 1, name.c_str(), -1, nullptr);
    int rc = sqlite3_step(insert_id_stmt_);
    if (rc != SQLITE_DONE) spdlog::critical("Failed to insert block ID of {}", name);
    else spdlog::debug("Block ID of {} inserted successfully", name);
    auto id = sqlite3_last_insert_rowid(db_);
    blocks_id_[name] = id;
    return id;
}
