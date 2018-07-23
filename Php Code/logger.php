<?php

$servername = "localhost"; //change this
$username = "MY_DATABASE_USER"; //change this
$password = "MY_DATABASE_PASSWORD"; //change this
$dbname = "MY_DATABASE_NAME"; //change this


$lat = 0;
$lng = 0;
$type = 0;
$name = 0;
$speed = 0;
$deg = 0;
$payload = 0;

if(isset($_GET['lat'])) $lat = $_GET['lat'];
if(isset($_GET['lng'])) $lng = $_GET['lng'];
if(isset($_GET['type'])) $type = $_GET['type'];
if(isset($_GET['name'])) $name = $_GET['name'];
if(isset($_GET['speed'])) $speed = $_GET['speed'];
if(isset($_GET['deg'])) $deg = $_GET['deg'];
if(isset($_GET['payload'])) $payload = $_GET['payload'];

// this code is the absolute minimum... for production systems i strongly recomend some kind of sql injection protection!
try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $sql = "INSERT INTO location (date_changed, lat, lng, type, name, course, speed, payload) VALUES 
(CURRENT_TIMESTAMP, '$lat', '$lng', '$type', '$name', '$deg', '$speed', '$payload')";
    $conn->exec($sql);
    echo 1;
}
catch(PDOException $e)
{
    echo 0;
}
$conn = null;

