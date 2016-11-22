add_executable(app
  src/app.c
  )

target_link_libraries(app
  Fever
  sdl2
  )
