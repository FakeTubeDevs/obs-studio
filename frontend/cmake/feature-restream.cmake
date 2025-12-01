if(RESTREAM_CLIENTID AND RESTREAM_HASH MATCHES "^(0|[a-fA-F0-9]+)$" AND TARGET OBS::browser-panels)
  # Disabled in FakeTube build
  target_disable_feature(obs-studio "Restream API connection")
else()
  target_disable_feature(obs-studio "Restream API connection")
  set(RESTREAM_CLIENTID "")
  set(RESTREAM_HASH "0")
endif()
