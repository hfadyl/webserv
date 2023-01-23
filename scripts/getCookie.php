<?php

if(!isset($_COOKIE["okay"])) {
    echo "Cookie named 'user' is not set!";
} else {
    echo "Cookie 'user' is set!<br>";
    echo "Value is: " . $_COOKIE['okay'];
}
?>