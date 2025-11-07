#include "append.hpp"

using json = nlohmann::json;
using hyperlog::rid_cache::RID_Cache;

namespace routes
{
void HandleSingularAppend(const httplib::Request& req, httplib::Response& res)
{
  static RID_Cache cache;
  static hyperlog::idgen::IDGen idgen;

  std::string key;
  std::string data;

  try
  {
    auto body = json::parse(req.body);
    if (!body.contains("key") || !body.contains("data"))
    {
      res.status = 400;
      res.set_content(R"({"error":"missing_fields"})", "application/json");
      return;
    }

    key = body["key"].get<std::string>();
    data = body["data"].get<std::string>();
  }
  catch (...)
  {
    res.status = 400;
    res.set_content(R"({"error":"invalid_json"})", "application/json");
    return;
  }

  std::string rid = req.has_header(hyperlog::constants::RID_HEADER)
                        ? req.get_header_value(hyperlog::constants::RID_HEADER)
                        : idgen.hex(32);

  std::string stream = req.has_header(hyperlog::constants::STREAM_HEADER)
                           ? req.get_header_value(hyperlog::constants::STREAM_HEADER)
                           : hyperlog::constants::DEFAULT_STREAM;

  int deadline = req.has_header(hyperlog::constants::DEADLINE_HEADER)
                     ? std::stoi(req.get_header_value(hyperlog::constants::DEADLINE_HEADER))
                     : 0;

  uint32_t crc = util::CRC32C::compute(data.data(), data.size());

  auto cached = cache.get(key);
  if (cached.found)
  {
    if (cached.value == crc)
    {
      res.status = 201;
      json j;
      j["status"] = "exists";
      j["rid"] = rid;
      res.set_content(j.dump(), "application/json");
      return;
    }
    else
    {
      res.status = 409;
      json j;
      j["error"] = "conflict";
      j["rid"] = rid;
      res.set_content(j.dump(), "application/json");
      return;
    }
  }

  cache.set(key, crc);

  json result;
  result["status"] = "appended";
  result["rid"] = rid;
  result["stream"] = stream;
  result["crc32c"] = crc;
  result["deadline_us"] = deadline;

  res.status = 201;
  res.set_content(result.dump(), "application/json");
}
}