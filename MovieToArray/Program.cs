using System;
using System.Drawing;
using Microsoft.Expression.Encoder;

namespace MovieToArray {
    class Program {
        static void Main(string[] args) {
            const int height = 120, width = 160;
            const double fps = 10.0;
            var avFile = new AudioVideoFile(args[0]);
            var generator = avFile.CreateThumbnailGenerator(avFile.VideoStreams[0].VideoSize);
            var totalTime = avFile.VideoStreams[0].Duration.TotalMilliseconds;
            int numFrame = (int)(fps * totalTime / 1000);
            var bin = new byte[numFrame * height * width * 2];
            for (int k = 0; k < numFrame; ++k) {
                var bitmap = new Bitmap(generator.CreateThumbnail(TimeSpan.FromSeconds(k/fps)), width, height);
                for (int i = 0; i < bitmap.Height; ++i) {
                    for (int j = 0; j < bitmap.Width; ++j) {
                        uint argb = (uint)bitmap.GetPixel(j, bitmap.Height - 1 - i).ToArgb();
                        uint r = (argb >> (16 + 3)) & 0x1F;
                        uint g = (argb >> (8 + 2)) & 0x3F;
                        uint b = (argb >> (0 + 3)) & 0x1F;
                        ushort rgb = (ushort)((r << (6 + 5)) | (g << 5) | b);
                        bin[k * bitmap.Width * bitmap.Height * 2 + 2 * (bitmap.Width * i + j) + 1] = (byte)(rgb >> 8);
                        bin[k * bitmap.Width * bitmap.Height * 2 + 2 * (bitmap.Width * i + j)] = (byte)rgb;
                    }
                }
                bitmap.Dispose();
            }
            var fs = new System.IO.FileStream(System.IO.Path.ChangeExtension(args[0], "dat"), System.IO.FileMode.Create, System.IO.FileAccess.Write);
            fs.Write(bin, 0, bin.Length);
            fs.Close();
            generator.Dispose();
        }
    }
}
