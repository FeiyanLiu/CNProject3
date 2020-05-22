<?php
    header('Content-Type: text/html; charset=gb2312');
	$command = 'Project1.exe '.$_POST['name'].' '.$_POST['pwd'].' '.$_POST['serial'];
    $result = passthru($command);
    print_r($result);
?>