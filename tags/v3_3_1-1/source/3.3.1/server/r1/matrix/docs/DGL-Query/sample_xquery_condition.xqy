
let $y := if( #THIS.DGL#//
              FlowStatusResponse[@flowID = 'flow2']/
              StepStatusResponse[@stepID = 'f2s1']/
              Variables/
              part[@name = 'maker']/
              anyVar/text() = 'OS')
              then true
              else false


return $y
