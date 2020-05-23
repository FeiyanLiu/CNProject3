<?php
    header('Content-Type: text/html; charset=gb2312');
	$command = 'CNproject3-php.exe '.$_POST['name'].' '.$_POST['pwd'].' '.$_POST['serial'];
    $result = passthru($command);
    print_r($result);
?>