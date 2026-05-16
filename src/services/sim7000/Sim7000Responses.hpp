#pragma once

struct Sim7000Responses {
  bool networkRegistered = false;
  bool lastResponseOk = false;
  bool lastResponseReady = false;
  bool PDPActive = false;
  bool moduleReady = false;
};