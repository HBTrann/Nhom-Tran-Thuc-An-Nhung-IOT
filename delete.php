<?php
	include('config.php');
	mysqli_query($conn,"delete from diemdanh");
	header('location:diemdanh1.php');
?>