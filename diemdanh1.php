
<!DOCTYPE html>
<html>
<head>
    <title>HỆ THỐNG ĐIỂM DANH SINH VIÊN</title>
    <meta charset="utf-8">
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/css/bootstrap.min.css">
    <style>
        body{ font:14px san-serif;}
        .wrapper{float:left; width: 400px; padding:20px; margin: 20px;}
        .table_width{margin:auto; width:70%;}
    </style>
    <!-- <script
    src="https://code.jquery.com/jquery-3.6.0.min.js"
    integrity="sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4="
    crossorigin="anonymous"></script> -->
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    <link rel="stylesheet" href="css/reset.css">
</head>
<body>
    <style>
        body{
            font-size: 16px;
        }
        #wrapper{
            max-width: 960px;
            margin: 0px auto;
        }
        /* Style nav tabs*/
        .tabs{
            border-bottom: 1px solid #ddd;
        }
        .nav-tabs{
            display: flex;
            list-style: none;
            margin: 0px;
            padding: 0;
            border-bottom: 3px solid #ddd;
        }
        .nav-tabs li{
            margin-right: 10px;
        }
        .nav-tabs li a{
            display: block;
            padding: 6px 10px;
            text-decoration: none;
            position: relative;
        }
        .nav-tabs li a::after{
            content: "";
            height: 3px;
            width: 100%;
            position: absolute;
            left: 0px;
            bottom: -3px;
            background: transparent; 
        }
        .nav-tabs li.active a::after, .nav-tabs li:hover a::after{
            background: #e74c3c;
        }

        /* STYLE tab-content*/
        .tab-content{
            padding: 20px 0px;
        }

    </style>
    <div id = "wrapper">
        <h1 style="background-color: #80ced6;">TRƯỜNG ĐẠI HỌC SƯ PHẠM KỸ THUẬT TPHCM</h1>
        <div class="tabs">
            <ul class="nav-tabs">
                <li class="active"><a href="#menu_1">Danh sách lớp</a></li>
                <li ><a href="#menu_2">Điểm danh không thẻ</a></li>
                <!-- <li ><a href="#menu_3">Chưa nghĩ ra</a></li> -->
            </ul>
            <div class="tab-content">
                <div id="menu_1" class="tab-content-item">
                    
                    <form class="form-horizontal well" action="import.php" method="post" name="upload_excel" enctype="multipart/form-data">
                        <fieldset>
                            
                            <div class="control-group">
                                <div class="control-label">
                                    
                                </div>
                                <div class="controls">
                                    <input type="file" name="file" id="file" class="input-large">
                                </div>
                            </div>
                            <br>
                            <div class="control-group">
                                <div class="controls">
                                <button type="submit" id="submit" name="Import" class="btn btn-primary button-loading" data-loading-text="Loading...">Upload</button>
                                </div>
                            </div>
                        </fieldset>
                    </form>
                    <br>
                    <div class="row justify-content-end my-2">
                    <div class="col-md-4 text-end">
                        <a href="export.php" class="btn btn-primary rounded-0 shadow" target="_blank">Export to CSV</a>
                    </div>
                    </div>
                    <br>
                    
                    <table id="mytable" class="table table-dark table-striped">
                        <tr>
                            <th>STT</th>
                            <th>HỌ VÀ TÊN</th> 
                            <th>MSSV</th> 
                            <th>LỚP</th> 
                            <th>NGÀY</th>
                            <th>THỜI GIAN VÀO</th>
                            <th>THỜI GIAN RA</th> 
                        </tr>
                        <tr>
                        <?php
                            //connect to database
                            include("config.php");
                            //Doc du lieu tu database
                            $sql = "select stt, hoten, mssv, ngay, timein, timeout, lop from diemdanh";
                            $result = mysqli_query($conn,$sql);
                            //Gui du lieu len website
                            if ($result->num_rows > 0) 
                            {
                                // Load dữ liệu lên website
                                while($row = $result->fetch_assoc()) 
                                {
                                echo "<tr><td>" . $row["stt"]. "</td><td>". $row["hoten"]."</td><td>". $row["mssv"]."</td><td>". $row["lop"]. "</td><td>". $row["ngay"]. "</td><td>". $row["timein"]. "</td><td>". $row["timeout"]. "</td></tr>" ;        
        
                                }
                                echo "</table>";
                            } 
                            else 
                            {
                                echo "0 results";
                            }
                            //echo $result1[0];
                            //if($result1 = mysqli_query())
                            //close mysql
                            mysqli_close($conn);
        
                        ?>
                        </tr>
                        
                    </table>
                    <br>
                    <div class="row justify-content-end my-2">
                    <div class="col-md-4 text-end">
                        <a href="delete.php" class="btn btn-primary rounded-0 shadow" target="_blank">Delete Data</a>
                    </div>
                    </div>
                    <br>
                </div>
                <div id="menu_2" class="tab-content-item">
                    <h2>Nhập thông tin để điểm danh</h2>
                    <p>Vui lòng điền thông tin</p>
                    <?php
                        // Include config file
                        include("config.php");
                        
                        // Define variables and initialize with empty values
                        $hoten = $mssv = $lop = "";
                        $hoten_err = $mssv_err = $lop_err = "";
                        
                        // Processing form data when form is submitted
                        if($_SERVER["REQUEST_METHOD"] == "POST"){
                        
                            // Validate hoten
                            if(empty(trim($_POST["hoten"]))){
                                $hoten_err = "Vui lòng nhập họ và tên.";
                            } else{
                                $temp_hoten = trim($_POST["hoten"]);
                                $sql = "SELECT*FROM diemdanh WHERE hoten = '$temp_hoten'";
                                //$sql = "SELECT*FROM diemdanh WHERE hoten = '$temp_hoten'";
                                $result = mysqli_query($conn,$sql);
                                $count = mysqli_num_rows($result);
                                if($count == 1) {
                                    //$hoten_err = "Người này đã điểm danh.";
                                    $sql1 = "update diemdanh set timein = CURRENT_TIMESTAMP, ngay = CURRENT_TIMESTAMP where hoten = ('$temp_hoten') ";
                                    //$sql1 = "update diemdanh set timeout = CURRENT_TIMESTAMP, ngay = CURRENT_TIMESTAMP where hoten = ('$temp_hoten') ";
                                    mysqli_query($conn, $sql1);
                                    mysqli_close($conn);  
                                }
                                else{
                                    $hoten = trim($_POST["hoten"]);
                                }
                                
                            }
                            // Validate mssv
                            if(empty(trim($_POST["mssv"]))){
                                $mssv_err = "Vui lòng nhập MSSV.";     
                            } elseif(strlen(trim($_POST["mssv"])) < 8){
                                $mssv_err = "Mã số sinh viên không đúng.";
                            } else{
                                $mssv = trim($_POST["mssv"]);
                            }
                            
                            // Validate lop
                            if(empty(trim($_POST["lop"]))){
                                $lop_err = "Vui lòng nhập lớp.";     
                            } else{
                                $lop = trim($_POST["lop"]);
                            }
                            
                            // Check input errors before inserting in database
                            // if(empty($hoten_err) && empty($mssv_err) && empty($lop_err)){
                            //     // Create SQL command
                            //     $sql = "insert into read_rc522e(hoten,mssv,lop) values ('$hoten','$mssv','$lop')";
                            //     //$sql = "update diemdanh set timein = CURRENT_TIMESTAMP, ngay = CURRENT_TIMESTAMP where hoten = ('$hoten') ";
                            //     mysqli_query($conn, $sql);

                            //     // disconnect from database
                            //     mysqli_close($conn);    
                            // }
                        
                        }
                    ?>
                    <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
                        <div class="form-group">
                            <label>Họ và tên</label>
                            <input type="text" name="hoten" class="form-control <?php echo (!empty($hoten_err)) ? 'is-invalid' : ''; ?>" value="<?php echo $username; ?>">
                            <span class="invalid-feedback"><?php echo $username_err; ?></span>
                        </div>
                        <br>    
                        <div class="form-group">
                            <label>Mã số sinh viên</label>
                            <input type="text" name="mssv" class="form-control <?php echo (!empty($mssv_err)) ? 'is-invalid' : ''; ?>" value="<?php echo $password; ?>">
                            <span class="invalid-feedback"><?php echo $password_err; ?></span>
                        </div>
                        <br>
                        <div class="form-group">
                            <label>Lớp</label>
                            <input type="text" name="lop" class="form-control <?php echo (!empty($lop_err)) ? 'is-invalid' : ''; ?>" value="<?php echo $password; ?>">
                            <span class="invalid-feedback"><?php echo $password_err; ?></span>
                        </div>
                        <br>
                        <div class="form-group">
                            <input type="submit" class="btn btn-primary" value="Điểm danh">
                        </div>
                    </form>
                </div>
                <!-- <div id="menu_3" class="tab-content-item">
                    <h2>Danh sách các lớp học phần đang giảng dạy</h2>
                    <p id="19146CL2">Lớp 19146CL2.</p>

                </div> -->
            </div>
        </div>

    </div>

</body>

<script src="https://code.jquery.com/jquery-3.6.0.js" integrity="sha256-H+K7U5CnXl1h5ywQfKtSj8PCmoN9aaq30gDh27Xc0jk=" crossorigin="anonymous"></script>
<script>
    $(document).ready(function(){
        $('.tab-content-item').hide();
        $('.tab-content-item:first-child').fadeIn();
        $('.nav-tabs li').click(function(){
            //Active nav tabs
            $('.nav-tabs li').removeClass('active');
            $(this).addClass('active');

            //Show tab-content
            let id_tab_content = $(this).children('a').attr('href');
            $('.tab-content-item').hide();
            $(id_tab_content).fadeIn();
            return false;
        });
    });
</script>
<script>
document.getElementById("19146CL2").addEventListener("click", myFunction);

function myFunction() {
  document.getElementById("19146CL2").innerHTML = "YOU CLICKED ME!";
}
</script>


</html>