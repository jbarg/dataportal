<?php
function array_serialize(&$arr,$pos) {
   $arr = serialize($arr);
}

function array_unserialize(&$arr,$pos) {
   $arr = unserialize($arr);
}

function ndimension_array_diff(&$arr1, &$arr2) {
   // make a copy
   $first_array_s = $arr1;
   $second_array_s = $arr2;

   // serialize all sub-arrays
   array_walk($first_array_s,'array_serialize');
   array_walk($second_array_s,'array_serialize');

   // array_diff the serialized versions
   $diff = array_diff($first_array_s,$second_array_s);

   // unserialize the result
   array_walk($diff,'array_unserialize');

   return $diff;
}
?>
