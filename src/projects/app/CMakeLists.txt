add_executable(app
  src/app.cpp
  )

target_link_libraries(app
  Fever
  )

target_compile_features(app
  PRIVATE cxx_nullptr
  )
