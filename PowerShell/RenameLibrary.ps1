# This PowerShell script shall rename all the texture files located in the texture library folder.
# Change PowerShell script execution policy : Set-ExecutionPolicy RemoteSigned 

# Destination folder
$folderPath = "..\LigidPainter\Resources\Texture Library"

# Define file extensions in an array
$fileExtensions = "*.jpg", "*.jpeg"

# Get files, filter by multiple extensions, and sort them alphabetically by name
$files = Get-ChildItem -Path $folderPath -Recurse -Include $fileExtensions | Sort-Object -Property Name

# If the files array is empty
if ($files.Count -eq 0) {
    Write-Host "No files found in the specified directory."
}    

# Loop through all the files in the Texture Library
$index = 0
foreach ($file in $files) {
    # If the file exists
    if (Test-Path $file.FullName) {
        # If the FileSystem element is file
        if(Test-Path $file.FullName -PathType Leaf){
            $newName = ($index).ToString() + ".jpg"
            
            # If the file was not already indexed
            if($file.Name -ne $newName){
                # Check if the new name already exists, increment index until a unique name is found
                while (Test-Path (Join-Path $folderPath $newName)) {
                    $index++
                    $newName = ($index).ToString() + ".jpg"
                }
    
                # Rename the texture file
                $renamedFile = Rename-Item -Path $file.FullName -NewName $newName
                
                # Rename-Item was successfully executed
                if ($renamedFile) {
                    Write-Host "File renamed successfully: $($file.FullName) -> $($renamedFile.FullName)"
                    $index++
                } 
                # Rename-Item was NOT executed
                else {
                    Write-Host "Failed to rename file: $($file.FullName) to $($newName)"
                    # Print the error message
                    if ($Error) {
                        Write-Host "Error message: $($Error[0].Exception.Message)"
                    }
                }
            }

        }
    }
    # If the current file doesn't exist
    else{
        Write-Host "Error : Current file path doesn't exist : " + $file.FullName
    }
}