<?php 
// Load the database connection
include 'config.php';
 
//CSV Filename
$fname = 'diemdanh.csv';
 
// Header Row Data: Array
$header = ["STT","Ho va ten", "MSSV", "Lop", "Ngay", "Timein", "Timeout"];
 
// Selecting all authors query
$qry = $conn->query("SELECT * FROM diemdanh order by stt asc");
 
if($qry->num_rows <= 0){
    echo "<script> alert('No data has fetched.'); location.href = './'; </script>";
    exit;
}
 
//Open a File
$file = fopen("php://memory","w");
 
// Attach Header
fputcsv($file, $header,',');
 
 
// Loop the authors and put it into the CSV file
while($row = $qry->fetch_assoc()){
    fputcsv($file, [$row['stt'], $row['hoten'], $row['mssv'], $row['lop'], $row['ngay'], $row['timein'], $row['timeout']],',');
}
 
fseek($file,0);
 
// Add headers to download the file
header('Content-Type: text/csv');
header('Content-Disposition: attachment; filename="'.$fname.'";');
 
// Read File 
fpassthru($file);
exit;