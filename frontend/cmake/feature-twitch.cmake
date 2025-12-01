if(TWITCH_CLIENTID AND TWITCH_HASH MATCHES "^(0|[a-fA-F0-9]+)$" AND TARGET OBS::browser-panels)
  # Disabled in FakeTube build
  target_disable_feature(obs-studio "Twitch API connection")
else()
  target_disable_feature(obs-studio "Twitch API connection")
  set(TWITCH_CLIENTID "")
  set(TWITCH_HASH "0")
endif()
