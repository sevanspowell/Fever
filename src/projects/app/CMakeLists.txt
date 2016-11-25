add_executable(app
  src/app.c
  )

target_link_libraries(app
  PRIVATE Fever
  PRIVATE sdl2
  PRIVATE stb
  )
