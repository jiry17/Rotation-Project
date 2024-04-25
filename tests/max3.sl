(set-logic LIA)

(synth-fun max3 ((x Int) (y Int) (z Int)) Int
    ((Start Int (x
                 y
                 z
                 0
                 1
                 (+ Start Start)
                 (- Start Start)
                 (ite StartBool Start Start)))
     (StartBool Bool ((and StartBool StartBool)
                      (or  StartBool StartBool)
                      (not StartBool)
                      (<=  Start Start)
                      (=   Start Start)
                      (>=  Start Start)))))

(constraint (= (max3 0 0 0) 0))
(constraint (= (max3 0 0 1) 1))
(constraint (= (max3 0 0 2) 2))
(constraint (= (max3 0 1 0) 1))
(constraint (= (max3 0 1 1) 1))
(constraint (= (max3 0 1 2) 2))
(constraint (= (max3 0 2 0) 2))
(constraint (= (max3 0 2 1) 2))
(constraint (= (max3 0 2 2) 2))
(constraint (= (max3 1 0 0) 1))
(constraint (= (max3 1 0 1) 1))
(constraint (= (max3 1 0 2) 2))
(constraint (= (max3 1 1 0) 1))
(constraint (= (max3 1 1 1) 1))
(constraint (= (max3 1 1 2) 2))
(constraint (= (max3 1 2 0) 2))
(constraint (= (max3 1 2 1) 2))
(constraint (= (max3 1 2 2) 2))
(constraint (= (max3 2 0 0) 2))
(constraint (= (max3 2 0 1) 2))
(constraint (= (max3 2 0 2) 2))
(constraint (= (max3 2 1 0) 2))
(constraint (= (max3 2 1 1) 2))
(constraint (= (max3 2 1 2) 2))
(constraint (= (max3 2 2 0) 2))
(constraint (= (max3 2 2 1) 2))
(constraint (= (max3 2 2 2) 2))

(check-synth)

