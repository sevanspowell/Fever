add_executable(app
  src/app.cpp
  )

target_link_libraries(app
  Fever
  sdl2
  )

target_compile_features(app
  PRIVATE cxx_nullptr
  )
