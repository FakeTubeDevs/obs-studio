if(
  YOUTUBE_CLIENTID
  AND YOUTUBE_SECRET
  AND YOUTUBE_CLIENTID_HASH MATCHES "^(0|[a-fA-F0-9]+)$"
  AND YOUTUBE_SECRET_HASH MATCHES "^(0|[a-fA-F0-9]+)$"
  AND TARGET OBS::browser-panels
)
  # Disabled in FakeTube build
  target_disable_feature(obs-studio "YouTube API connection")
else()
  target_disable_feature(obs-studio "YouTube API connection")
  set(YOUTUBE_SECRET_HASH 0)
  set(YOUTUBE_CLIENTID_HASH 0)
endif()
