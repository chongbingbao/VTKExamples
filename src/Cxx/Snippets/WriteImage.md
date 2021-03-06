# Description

Write the render window view to an image file.

## C++

### Declaration Section

```c++

#include <algorithm>
#include <array>
#include <locale>
#include <string>

namespace
{
/**
 * Write the render window view to an image file.
 * 
 * Image types supported are:
 *  BMP, JPEG, PNM, PNG, PostScript, TIFF.
 * The default parameters are used for all writers, change as needed.
 * 
 * @param fileName The file name, if no extension then PNG is assumed.
 * @param renWin The render window.
 * @param rgba Used to set the buffer type.
 * 
 */
void WriteImage(std::string const& fileName, vtkRenderWindow* renWin,
                bool rgba = true);


}

```

### Implementation Section

``` c++

namespace
{
void WriteImage(std::string const& fileName, vtkRenderWindow* renWin, bool rgba)
{
  if (!fileName.empty())
  {
    std::string fn = fileName;
    std::string path;
    std::string ext;
    std::size_t found = fn.find_last_of(".");
    if (found == std::string::npos)
    {
      path = fn;
      ext = ".png";
      fn += ext;
    }
    else
    {
      path = fileName.substr(0, found);
      ext = fileName.substr(found, fileName.size());
    }
    std::locale loc;
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [=](char const& c) { return std::tolower(c, loc); });
    vtkSmartPointer<vtkImageWriter> writer =
      vtkSmartPointer<vtkImageWriter>::New();
    if (ext == ".bmp")
    {
      writer = vtkSmartPointer<vtkPNGWriter>::New();
    }
    else if (ext == ".jpg")
    {
      writer = vtkSmartPointer<vtkPNGWriter>::New();
    }
    else if (ext == ".pnm")
    {
      writer = vtkSmartPointer<vtkPNGWriter>::New();
    }
    else if (ext == ".ps")
    {
      if (rgba)
      {
        rgba = false;
      }
      writer = vtkSmartPointer<vtkPostScriptWriter>::New();
    }
    else if (ext == ".tiff")
    {
      writer = vtkSmartPointer<vtkTIFFWriter>::New();
    }
    else
    {
      writer = vtkSmartPointer<vtkPNGWriter>::New();
    }
    vtkSmartPointer<vtkWindowToImageFilter> window_to_image_filter =
      vtkSmartPointer<vtkWindowToImageFilter>::New();
    window_to_image_filter->SetInput(renWin);
    window_to_image_filter->SetScale(1); // image quality
    if (rgba)
    {
      window_to_image_filter->SetInputBufferTypeToRGBA();
    }
    else
    {
      window_to_image_filter->SetInputBufferTypeToRGB();
    }
    // Read from the front buffer.
    window_to_image_filter->ReadFrontBufferOff();
    window_to_image_filter->Update();

    writer->SetFileName(fn.c_str());
    writer->SetInputConnection(window_to_image_filter->GetOutputPort());
    writer->Write();
  }
  else
  {
    std::cerr << "No filename provided." << std::endl;
  }

  return;
}

```

### Usage

``` c++

  WriteImage(f, renWin, false);
  
```
