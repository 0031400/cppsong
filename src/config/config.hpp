#pragma once
#include "utils/headers.hpp"
#include <string>
#include <vector>
class TlsClientConfig {
  bool enabled;
  bool insecure;
  std::string serverName;
};
class TlsServerConfig {
  bool enabled;
  std::string certificatePath;
  std::string keyPath;
};
class DnsServerConfig {
  std::string tag;
  std::string type;
  std::string server;
  u16 server_port;
  std::string path;
  TlsClientConfig tls;
};
class DnsConfig {
  std::string listen;
  u16 listen_port;
};
class TransportConfig {
  std::string type;
  std::string path;
};
class UserPassConfig {
  std::string username;
  std::string password;
};
class InboundConfig {
  std::string tag;
  std::string type;
  std::string listen;
  u16 listen_port;
  std::vector<std::string> users;
  std::vector<std::string> uuids;
  TlsServerConfig tls;
  std::string tunIpv4;
  std::string tunIpv6;
  std::string tunNextIpv4;
  std::string tunNextIpv6;
};
class OutboundConfig {
  std::string tag;
  std::string type;
  std::string server;
  u16 server_port;
  std::string uuid;
  TlsClientConfig tls;
};
class DomainRuleConfig {
  std::vector<std::string> domain;
  std::vector<std::string> domain_suffix;
  std::vector<std::string> domain_keyword;
  std::vector<std::string> domain_regex;
};
class RouteRuleConfig {
  DomainRuleConfig rule;
  std::vector<std::string> rule_set;
  std::string outbound;
};
class RuleSetConfig {
  std::string tag;
  std::string type;
  std::string path;
  std::string format;
};
class RouterConfig {
  std::vector<RouteRuleConfig> rules;
  std::vector<RuleSetConfig> ruleSets;
  std::string final;
};
class AppConfig {
  std::vector<InboundConfig> inbounds;
  std::vector<OutboundConfig> outbounds;
  RouterConfig router;
  DnsConfig dns;
};