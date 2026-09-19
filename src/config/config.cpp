#include "config/config.hpp"
#include "utils/json.hpp"
namespace {

TlsClientConfig parseTlsClientConfig(const json::object &obj) {
  TlsClientConfig v;
  v.enabled = getBool(obj, "enabled");
  v.insecure = getBool(obj, "insecure");
  v.serverName = getString(obj, "server_name");
  return v;
}
TlsServerConfig parseTlsServerConfig(const json::object &obj) {
  TlsServerConfig v;
  v.enabled = getBool(obj, "enabled");
  v.keyPath = getBool(obj, "key_path");
  v.certificatePath = getString(obj, "certificate_path");
  return v;
}
TransportConfig parseTransportConfig(const json::object &obj) {
  TransportConfig v;
  v.type = getString(obj, "type");
  v.path = getString(obj, "path");
  v.host = getString(obj, "host");
  return v;
}
UserPassConfig parseUserPassConfig(const json::object &obj) {
  UserPassConfig v;
  v.username = getString(obj, "username");
  v.password = getString(obj, "password");
  return v;
}
InboundConfig parseInboundConfig(const json::object &obj) {
  InboundConfig v;
  v.tag = getString(obj, "tag");
  v.type = getString(obj, "type");
  v.listen = getString(obj, "listen");
  v.listen_port = getu16(obj, "listen_port");
  auto array = getArray(obj, "users");
  for (auto item : array) {
    v.users.emplace_back(parseUserPassConfig(item.as_object()));
  }
  v.uuids = getStringList(obj, "uuids");
  v.tunIpv4 = getString(obj, "tunIpv4");
  v.tunIpv6 = getString(obj, "tunIpv6");
  v.tunNextIpv4 = getString(obj, "tunNextIpv4");
  v.tunNextIpv6 = getString(obj, "tunNextIpv6");
  return v;
}
OutboundConfig parseOutboundConfig(const json::object &obj) {
  OutboundConfig v;
  v.tag = getString(obj, "tag");
  v.type = getString(obj, "type");
  v.server = getString(obj, "server");
  v.server_port = getu16(obj, "server_port");
  v.tls = parseTlsClientConfig(getObj(obj, "tls"));
  v.user = parseUserPassConfig(getObj(obj, "user"));
  v.transport = parseTransportConfig(getObj(obj, "transport"));
  v.uuid = getString(obj, "uuid");
  return v;
}
RuleSetConfig parseRuleSetConfig(const json::object &obj) {
  RuleSetConfig v;
  v.tag = getString(obj, "tag");
  v.format = getString(obj, "format");
  v.path = getString(obj, "path");
  v.type = getString(obj, "type");
  return v;
}
DomainRuleConfig parseDomainRuleConfig(const json::object &obj) {
  DomainRuleConfig v;
  v.domain = getStringList(obj, "domain");
  v.domainSuffix = getStringList(obj, "domain_suffix");
  v.domainKeyword = getStringList(obj, "domain_keyword");
  v.domainRegex = getStringList(obj, "domain_regex");
  return v;
}
RuleConfig parseRuleConfig(const json::object &obj) {
  RuleConfig v;
  v.domainRule = parseDomainRuleConfig(obj);
  v.cidr = getStringList(obj, "cidr");
  return v;
}
RouteRuleConfig parseRouteRuleConfig(const json::object &obj) {
  RouteRuleConfig v;
  v.rule = parseRuleConfig(obj);
  v.rule_set = getStringList(obj, "rule_set");
  v.outbound = getString(obj, "outbound");
  return v;
}
RouterConfig parseRouterConfig(const json::object &obj) {
  RouterConfig v;
  v.final = getString(obj, "final");
  auto array = getArray(obj, "rules");
  for (auto item : array) {
    v.rules.emplace_back(parseRouteRuleConfig(item.as_object()));
  }
  array = getArray(obj, "rule_set");
  for (auto item : array) {
    v.ruleSets.emplace_back(parseRuleSetConfig(item.as_object()));
  }
  return v;
}
DnsServerConfig parseDnsServerConfig(const json::object &obj) {
  DnsServerConfig v;
  v.server = getString(obj, "server");
  v.server_port = getu16(obj, "server_port");
  v.tag = getString(obj, "tag");
  v.type = getString(obj, "type");
  v.tls = parseTlsClientConfig(getObj(obj, "tls"));
  v.path = getString(obj, "path");
  return v;
}
DnsRuleConfig parseDnsRuleConfig(const json::object &obj) {
  DnsRuleConfig v;
  v.rule = parseDomainRuleConfig(obj);
  v.rule_set = getStringList(obj, "rule_set");
  v.server = getString(obj, "server");
  return v;
}
DnsConfig parseDnsConfig(const json::object &obj) {
  DnsConfig v;
  auto array = getArray(obj, "servers");
  for (auto item : array) {
    v.servers.emplace_back(parseDnsServerConfig(item.as_object()));
  }
  array = getArray(obj, "rules");
  for (auto item : array) {
    v.rules.emplace_back(parseDnsRuleConfig(item.as_object()));
  }
  v.listen = getString(obj, "listen");
  v.listen_port = getu16(obj, "listen_port");
  v.final = getString(obj, "final");
  return v;
}
} // namespace
AppConfig paresAppConfig(const json::object &obj) {
  AppConfig v;
  auto array = getArray(obj, "inbounds");
  for (auto item : array) {
    v.inbounds.emplace_back(parseInboundConfig(item.as_object()));
  }
  array = getArray(obj, "outbounds");
  for (auto item : array) {
    v.outbounds.emplace_back(parseOutboundConfig(item.as_object()));
  }
  v.router = parseRouterConfig(getObj(obj, "router"));
  v.dns = parseDnsConfig(getObj(obj, "dns"));
  return v;
}