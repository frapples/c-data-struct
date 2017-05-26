((c-mode . ((eval . (let ((clang-args
                           (list "-Wall"
                                 "-Wextra"
                                 "-Werror"
                                 "-fexceptions"
                                 (concat "-I" (projectile-project-root) "../cmockery")
                                 (concat "-I"  (projectile-project-root) "lib"))))
                      (setq company-clang-arguments clang-args
                            flycheck-clang-args clang-args))))))
