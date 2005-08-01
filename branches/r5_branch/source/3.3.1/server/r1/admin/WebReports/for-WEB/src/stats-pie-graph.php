<?php
require_once ($jpgraph_path . "src/jpgraph.php");
require_once ($jpgraph_path . "src/jpgraph_pie.php");

function piegraph($labels, $tfc, $tfs, $fcdata, $fsdata, $title, $subtitle, $gfile) {
  $width=800;
  $height=800;
  $mycolors=array('aliceblue','bisque4','wheat1', 'salmon1', 'burlywood2',
                  'cadetblue3', 'chartreuse3', 'chocolate2', 'cyan3', 'cyan4',
                  'darkmagenta', 'darkolivegreen2', 'dodgerblue3', 'dodgerblue4', 'goldenrod',
                  'thistle2', 'gray4', 'khaki1', 'lemonchiffon1', 'lightsalmon',
                  'lightskyblue3', 'aquamarine3', 'steelblue4', 'darkorchid4', 'yellow');

  // Create the Pie Graph.
  $graph = new PieGraph($width,$height,'auto');

  // Setup title
  $graph->title->Set($title);
  $graph->title->SetFont(FF_FONT2,FS_BOLD);
  $graph->subtitle->Set($subtitle);
//  $graph->SetAntiAlias();

  // Setup legend
  $graph->legend->Pos(0.01,0.99,'left','bottom');
  $graph->legend->SetColumns(3);
  $graph->legend->SetFont(FF_FONT2);
  $graph->legend->SetMarkAbsSize(12);
  $graph->legend->SetShadow(False);

  // Create plots
  $size=0.24;
  $p1 = new PiePlot($fcdata);
  $p1->SetSliceColors($mycolors);
  $p1->SetLegends($labels);
  $p1->SetSize($size);
  $p1->SetCenter(0.25,0.45);
  $p1->value->SetFont(FF_FONT2);
  $p1->value->SetColor("darkred");
  $p1->SetLabelPos(0.7);
  $p1->title->Set("Number of Files\n" . $tfc);
  $p1->title->SetFont(FF_FONT2,FS_BOLD);
  $p1->title->SetMargin(15);
  $p1->title->Align("","","center");

  $p2 = new PiePlot($fsdata);
  $p2->SetSliceColors($mycolors);
  $p2->SetSize($size);
  $p2->SetCenter(0.75,0.45);
  $p2->value->SetFont(FF_FONT2);
  $p2->value->SetColor("darkred");
  $p2->SetLabelPos(0.7);
  $p2->title->Set("Gigabytes\n" . $tfs);
  $p2->title->SetFont(FF_FONT2,FS_BOLD);
  $p2->title->SetMargin(15);
  $p2->title->Align("","","center");

  $graph->Add($p1);
  $graph->Add($p2);

  $graph->Stroke($gfile);
}
?>

