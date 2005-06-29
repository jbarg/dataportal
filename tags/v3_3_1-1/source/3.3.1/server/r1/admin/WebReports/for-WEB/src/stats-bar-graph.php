<?php
require_once ($jpgraph_path . "src/jpgraph.php");
require_once ($jpgraph_path . "src/jpgraph_bar.php");

function sizegraph($datax, $datay, $dec_point, $title, $subtitle, $gfile) {
  $width=800;
  $height=600;

  // Set the basic parameters of the graph 
  $graph = new Graph($width,$height,'auto');
  $graph->SetScale("textlin");

  $top = 70;
  $bottom = 30;
  $left = 70;
  $right = 30;
  $graph->SetMargin($left,$right,$top,$bottom);

  // Setup title
  $graph->title->Set($title);
  $graph->title->SetFont(FF_FONT2,FS_BOLD);
  $graph->subtitle->Set($subtitle);

  // Setup X-axis
  $graph->xaxis->SetTickLabels($datax);
  $graph->xaxis->SetFont(FF_FONT2,FS_BOLD);

  // Some extra margin looks nicer
  $graph->xaxis->SetLabelMargin(5);

  // Add some grace to y-axis so the bars doesn't go
  // all the way to the end of the plot area
  $graph->yaxis->scale->SetGrace(5);
  if( $dec_point )
    $graph->yaxis->SetLabelFormat('%0.2f');
  else
    $graph->yaxis->SetLabelFormat('%d');
  $graph->yaxis->SetFont(FF_FONT2);

  // Setup graph colors
  $graph->SetMarginColor('white');
  $graph->SetColor("#EBEBEB");
  $graph->ygrid->SetColor("#666666");

  // We don't want to display Y-axis
  //$graph->yaxis->Hide();

  // Now create the Y bar pot
  $bplot = new BarPlot($datay);
  $bplot->SetFillColor("#333399");
  $bplot->SetShadow("#000066");
  $bplot->value->Show();
  $bplot->value->SetFont(FF_FONT2,FS_BOLD);
  $bplot->value->SetColor("black");
  if( $dec_point )
    $bplot->value->SetFormatCallback('number_format2');
  else
    $bplot->value->SetFormatCallback('number_format');

  // Add the bar to the graph
  $graph->Add($bplot);

  $graph->Stroke($gfile);
}

function number_format2($aLabel) {
    return number_format($aLabel, 2);
}

?>

