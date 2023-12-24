function GetFolderData {
    param (
        [array]$files,
        [ref]$lineCount,
        [ref]$fileCount
    )
    
    foreach ($file in $files) {
        $fileCount.Value++
        $lines = Get-Content -Path $file.FullName
        $lineCount.Value += $lines.Count
    }
}

$srcFolder = "..\LigidPainter\Main\src"
$thirdpartyFolder = "..\LigidPainter\Main\thirdparty"
$shadersFolder = "..\LigidPainter\Resources\Shaders"
$overallFolder = "..\LigidPainter"

$cpp_Files_src = Get-ChildItem -Path $srcFolder -Filter *.cpp -File -Recurse
$c_Files_src = Get-ChildItem -Path $srcFolder -Filter *.c -File -Recurse
$hpp_Files_src = Get-ChildItem -Path $srcFolder -Filter *.hpp -File -Recurse
$h_Files_src = Get-ChildItem -Path $srcFolder -Filter *.h -File -Recurse

$cpp_Files_third = Get-ChildItem -Path $thirdpartyFolder -Filter *.cpp -File -Recurse
$c_Files_third = Get-ChildItem -Path $thirdpartyFolder -Filter *.c -File -Recurse
$hpp_Files_third = Get-ChildItem -Path $thirdpartyFolder -Filter *.hpp -File -Recurse
$h_Files_third = Get-ChildItem -Path $thirdpartyFolder -Filter *.h -File -Recurse

$glsl_Files = Get-ChildItem -Path $shadersFolder -Recurse -Include *.frag, *.vert, *.glsl | Sort-Object -Property Name

$image_Files = Get-ChildItem -Path $overallFolder -Recurse -Include *.jpg, *.jpeg, *.png, *.ico | Sort-Object -Property Name
$all_Files = Get-ChildItem -Path $overallFolder -Recurse | Sort-Object -Property Name

$cpp_FileCount_src = 0
$cpp_LineCount_src = 0
$cpp_FileCount_third = 0
$cpp_LineCount_third = 0

$hpp_FileCount_src = 0
$hpp_LineCount_src = 0
$hpp_FileCount_third = 0
$hpp_LineCount_third = 0

$h_FileCount_src = 0
$h_LineCount_src = 0
$h_FileCount_third = 0
$h_LineCount_third = 0

$c_FileCount_src = 0
$c_LineCount_src = 0
$c_FileCount_third = 0
$c_LineCount_third = 0

$glsl_FileCount = 0
$glsl_LineCount = 0

$image_FileCount = $image_Files.Length

$all_FileCount = $all_Files.Length

GetFolderData -files ($cpp_Files_src) -lineCount ([ref]$cpp_LineCount_src) -fileCount ([ref]$cpp_FileCount_src)
GetFolderData -files ($c_Files_src) -lineCount ([ref]$c_LineCount_src) -fileCount ([ref]$c_FileCount_src)
GetFolderData -files ($hpp_Files_src) -lineCount ([ref]$hpp_LineCount_src) -fileCount ([ref]$hpp_FileCount_src)
GetFolderData -files ($h_Files_src) -lineCount ([ref]$h_LineCount_src) -fileCount ([ref]$h_FileCount_src)
GetFolderData -files ($cpp_Files_third) -lineCount ([ref]$cpp_LineCount_third) -fileCount ([ref]$cpp_FileCount_third)
GetFolderData -files ($c_Files_third) -lineCount ([ref]$c_LineCount_third) -fileCount ([ref]$c_FileCount_third)
GetFolderData -files ($hpp_Files_third) -lineCount ([ref]$hpp_LineCount_third) -fileCount ([ref]$hpp_FileCount_third)
GetFolderData -files ($h_Files_third) -lineCount ([ref]$h_LineCount_third) -fileCount ([ref]$h_FileCount_third)
GetFolderData -files ($glsl_Files) -lineCount ([ref]$glsl_LineCount) -fileCount ([ref]$glsl_FileCount)

$srcTotalFileCount = $cpp_FileCount_src + $c_FileCount_src + $hpp_FileCount_src + $h_FileCount_src
$srcTotalLineCount = $cpp_LineCount_src + $c_LineCount_src + $hpp_LineCount_src + $h_LineCount_src

$thirdTotalFileCount = $cpp_FileCount_third + $c_FileCount_third + $hpp_FileCount_third + $h_FileCount_third
$thirdTotalLineCount = $cpp_LineCount_third + $c_LineCount_third + $hpp_LineCount_third + $h_LineCount_third

# Get the total size of the folder
$STORAGE_totalSize = (Get-ChildItem -Path $overallFolder -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB

# Get the size of texture files (change the extensions as needed)
$STORAGE_textureSize = (Get-ChildItem -Path $overallFolder -Recurse -Include *.jpg, *.jpeg, *.png | Measure-Object -Property Length -Sum).Sum / 1MB

# Get the size of .txt files
$STORAGE_txtSize = (Get-ChildItem -Path $overallFolder -Recurse -Include *.txt, *.cpp, *.hpp, *.c, *.h, *.glsl, *.frag, *.vert, *.frag, *.json, *.md, *.LICENSE | Measure-Object -Property Length -Sum).Sum / 1MB


Write-Host "SRC C++ | File count : $cpp_FileCount_src | Line count : $cpp_LineCount_src"
Write-Host "SRC C | File count : $c_FileCount_src | Line count : $c_LineCount_src"
Write-Host "SRC H++ | File count : $hpp_FileCount_src | Line count : $hpp_LineCount_src"
Write-Host "SRC H | File count : $h_FileCount_src | Line count : $h_LineCount_src"

Write-Host ""
Write-Host "SRC TOTAL | File count : $($srcTotalFileCount) | Line count : $($srcTotalLineCount)"

Write-Host ""
Write-Host "--------------------------------------------------------------------------"
Write-Host ""

Write-Host "THIRD C++ | File count : $cpp_FileCount_third | Line count : $cpp_LineCount_third"
Write-Host "THIRD C | File count : $c_FileCount_third | Line count : $c_LineCount_third"
Write-Host "THIRD H++ | File count : $hpp_FileCount_third | Line count : $hpp_LineCount_third"
Write-Host "THIRD H | File count : $h_FileCount_third | Line count : $h_LineCount_third"

Write-Host ""
Write-Host "THIRD TOTAL | File count : $($thirdTotalFileCount) | Line count : $($thirdTotalLineCount)"

Write-Host ""
Write-Host "--------------------------------------------------------------------------"
Write-Host ""

Write-Host "GLSL | File count : $glsl_FileCount | Line count : $glsl_LineCount"

Write-Host ""
Write-Host "--------------------------------------------------------------------------"
Write-Host ""

Write-Host "OVERALL TOTAL (Shader codes excluded) | File count : $($srcTotalFileCount + $thirdTotalFileCount) | Line count : $($srcTotalLineCount + $thirdTotalLineCount)"
Write-Host ""
Write-Host "OVERALL TOTAL (Shader codes included) | File count : $($srcTotalFileCount + $thirdTotalFileCount + $glsl_FileCount) | Line count : $($srcTotalLineCount + $thirdTotalLineCount + $glsl_LineCount)"

Write-Host ""
Write-Host "--------------------------------------------------------------------------"
Write-Host ""

Write-Host "IMAGE FILES | File count : $image_FileCount"
Write-Host "OVERALL RESOURCE FILES | File count : $($all_FileCount - $thirdTotalFileCount - $srcTotalFileCount)"
Write-Host "ALL FILES | File count : $all_FileCount"

Write-Host ""
Write-Host "--------------------------------------------------------------------------"
Write-Host ""

Write-Host "IMAGE STORAGE | MEGA-BYTES : $STORAGE_textureSize"
Write-Host "TEXT FORMAT STORAGE | MEGA-BYTES : $STORAGE_txtSize"
Write-Host "OTHER | MEGA-BYTES : $($STORAGE_totalSize - $STORAGE_textureSize - $STORAGE_txtSize)"
Write-Host "TOTAL STORAGE | MEGA-BYTES : $STORAGE_totalSize"

Write-Host ""
Write-Host "--------------------------------------------------------------------------"
Write-Host ""