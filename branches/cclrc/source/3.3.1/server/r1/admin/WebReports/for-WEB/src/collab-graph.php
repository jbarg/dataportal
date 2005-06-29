<?php
require_once ($jpgraph_path . "src/jpgraph.php");
require_once ($jpgraph_path . "src/jpgraph_bar.php");

function sharegraph($datax, $datay1, $datay2, $datazero, $title, $subtitle, $gfile) {
  $width=800;
  $height=1200;

  // Set the basic parameters of the graph 
  $graph = new Graph($width,$height,'auto');
  $graph->SetScale("textlin");
  $graph->SetY2Scale("lin");

  $top = 100;
  $bottom = 30;
  $left = 140;
  $right = 30;
  $graph->Set90AndMargin($left,$right,$top,$bottom);

  // Setup title
  $graph->title->Set($title);
  $graph->title->SetFont(FF_FONT2,FS_BOLD);
  $graph->subtitle->Set($subtitle);
  $graph->legend->SetAbsPos(10,10,'right','top');

  // Setup X-axis
  $graph->xaxis->SetTitle('Sites','low');
  $graph->xaxis->title->SetFont(FF_FONT2,FS_BOLD);
  $graph->xaxis->SetTickLabels($datax);
  $graph->xaxis->SetFont(FF_FONT2,FS_BOLD);
  $graph->xgrid->Show(true);

  // Some extra margin looks nicer
  $graph->xaxis->SetLabelMargin(5);
  $graph->xaxis->SetTitleMargin(60);

  // Label align for X-axis
  $graph->xaxis->SetLabelAlign('right','center');

  // Add some grace to y-axis so the bars doesn't go
  // all the way to the end of the plot area
  $graph->yaxis->HideLabels();
  $graph->y2axis->HideLabels();
  $graph->yaxis->scale->SetGrace(10);
  $graph->y2axis->scale->SetGrace(10);
  //$graph->yaxis->SetLabelAlign('center','bottom');
  //$graph->yaxis->SetLabelFormat('%d');
  //$graph->yaxis->SetFont(FF_FONT2);
  //$graph->y2axis->SetFont(FF_FONT2);

  // Setup graph colors
  $graph->SetMarginColor('white');
  $graph->SetColor("#EBEBEB");
  $graph->yaxis->SetColor("#000066");
  $graph->y2axis->SetColor("#6666CC");
  $graph->xgrid->SetColor('black');
  $graph->ygrid->SetColor("#666666");

  // We don't want to display Y-axis
  //$graph->yaxis->Hide();

  // Create the "dummy" zero bplot
  $bplot0 = new BarPlot($datazero);

  // Now create the Y1 bar pot
  $bplot1 = new BarPlot($datay1);
  $bplot1->SetFillColor("#333399");
  $bplot1->SetShadow("#000066");
  $bplot1->value->Show();
  $bplot1->value->SetFont(FF_FONT1,FS_BOLD);
  $bplot1->value->SetAlign('left','center');
  $bplot1->value->SetColor("black");
  $bplot1->value->SetFormatCallback('number_format');
  $bplot1->SetLegend("Number of Files");
  $y1bplot = new GroupBarPlot(array($bplot1, $bplot0));

  // Now create the Y2 bar pot
  $bplot2 = new BarPlot($datay2);
  $bplot2->SetFillColor("#9999FF");
  $bplot2->SetShadow("#6666CC");
  $bplot2->value->Show();
  $bplot2->value->SetFont(FF_FONT1,FS_BOLD);
  $bplot2->value->SetAlign('left','center');
  $bplot2->value->SetColor("black");
  $bplot2->value->SetFormat('%.2f MB');
  $bplot2->SetLegend("Avg Size of Files in MB");
  $y2bplot = new GroupBarPlot(array($bplot0, $bplot2));

  // Add the bar to the graph
  $graph->Add($y1bplot);
  $graph->AddY2($y2bplot);


  $graph->Stroke($gfile);
}
?>
