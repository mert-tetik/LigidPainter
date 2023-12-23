# This PowerShell script shell resize all the textures inside of a folder then writes the resized files into the destination folder
# Change PowerShell script execution policy : Set-ExecutionPolicy RemoteSigned 

Add-Type -AssemblyName System.Drawing

# Variables
$textureFolder = "..\LigidPainter\Resources\Texture Library"
$destinationFolder = "" # Alter this
$fileExtensions = "*.jpg", "*.jpeg" # You can add other texture file extensions too  
$newWidth = 400
$newHeight = 400
$newExtension = [System.Drawing.Imaging.ImageFormat]::Jpeg

if(!(Test-Path $textureFolder)){
    throw "Invalid texture folder path"
}

if(!(Test-Path $destinationFolder)){
    throw "Invalid destination folder path"
}

# Get files, filter by multiple extensions, and sort them alphabetically by name
$files = Get-ChildItem -Path $textureFolder -Recurse -Include $fileExtensions | Sort-Object -Property Name

# If the files array is empty
if ($files.Count -eq 0) {
    Write-Host "No files found in the texture folder."
    return
}    

# Resize all the textures
foreach ($file in $files){
    # Load the image
    $image = [System.Drawing.Image]::FromFile($file)

    # Create a new bitmap with the new dimensions
    $bitmap = New-Object System.Drawing.Bitmap($newWidth, $newHeight)

    # Create a graphics object from the bitmap
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)

    # Draw the resized image
    $graphics.DrawImage($image, 0, 0, $newWidth, $newHeight)

    # Save the resized image to a new file
    $newImagePath = Join-Path $destinationFolder $file.Name
    Write-Host "Resized image path : $($newImagePath)"
    $bitmap.Save($newImagePath, $newExtension)

    # Dispose of objects
    $graphics.Dispose()
    $bitmap.Dispose()
    $image.Dispose()
}