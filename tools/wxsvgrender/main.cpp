#include <wx/wx.h>
#include <wxSVG/svg.h>

int main(int argc, char *argv[])
{
    if (!(argc == 3 || argc == 4 || argc == 5)) {
        printf("Usage:\n"
               "  wxsvgrender in.svg out.png\n"
               "  wxsvgrender in.svg out.png 500\n"
               "  wxsvgrender in.svg out.png 500 400\n");
        return 1;
    }

    int w = -1;
    if (argc == 4 || argc == 5) {
        w = wxAtoi(argv[3]);
    }

    int h = w;
    if (argc == 5) {
        h = wxAtoi(argv[4]);
    }

    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    auto doc = new wxSVGDocument();
    doc->Load(argv[1]);
    const auto img = doc->Render(w, h);
    img.SaveFile(argv[2]);

    return 0;
}
