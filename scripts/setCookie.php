<?php

    session_start();

    $_SESSION["name"] = "111";

    $cookie_name = "okay";
    $cookie_value = "maokish";
    setcookie("okay", "maokish", time() + (86400 * 30), "/"); // 86400 = 1 day


    echo "Session and cookies has been set    ";
?>