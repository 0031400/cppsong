#pragma once
#include "utils/headers.hpp"
#include "utils/json.hpp"
#include <string>
#include <vector>

struct TlsClientConfig {
  bool enabled;
  bool insecure;
  std::string serverName;
};
struct TlsServerConfig {
  bool enabled;
  std::string certificatePath;
  std::string keyPath;
};
struct DomainRuleConfig {
  std::vector<std::string> domain;
  std::vector<std::string> domainSuffix;
  std::vector<std::string> domainKeyword;
  std::vector<std::string> domainRegex;
};
struct DnsServerConfig {
  std::string tag;
  std::string type;
  std::string server;
  u16 server_port;
  std::string path;
  TlsClientConfig tls;
};
struct DnsRuleConfig {
  DomainRuleConfig rule;
  std::vector<std::string> rule_set;
  std::string server;
};
struct DnsConfig {
  std::string listen;
  u16 listen_port;
  std::vector<DnsServerConfig> servers;
  std::vector<DnsRuleConfig> rules;
  std::string final;
};
struct TransportConfig {
  std::string type;
  std::string path;
  std::string host;
};
struct UserPassConfig {
  std::string username;
  std::string password;
};
struct InboundConfig {
  std::string tag;
  std::string type;
  std::string listen;
  u16 listen_port;
  std::vector<UserPassConfig> users;
  std::vector<std::string> uuids;
  TlsServerConfig tls;
  std::string tunIpv4;
  std::string tunIpv6;
  std::string tunNextIpv4;
  std::string tunNextIpv6;
};
struct OutboundConfig {
  std::string tag;
  std::string type;
  std::string server;
  u16 server_port;
  UserPassConfig user;
  std::string uuid;
  TlsClientConfig tls;
};
struct RouteRuleConfig {
  DomainRuleConfig rule;
  std::vector<std::string> rule_set;
  std::string outbound;
};
struct RuleSetConfig {
  std::string tag;
  std::string type;
  std::string path;
  std::string format;
};
struct RouterConfig {
  std::vector<RouteRuleConfig> rules;
  std::vector<RuleSetConfig> ruleSets;
  std::string final;
};
struct AppConfig {
  std::vector<InboundConfig> inbounds;
  std::vector<OutboundConfig> outbounds;
  RouterConfig router;
  DnsConfig dns;
};
AppConfig paresAppConfig(const json::object &obj);