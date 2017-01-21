#include <Windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

SDL_Window *mainWindow;
SDL_Renderer *mainRenderer;
SDL_DisplayMode display;
SDL_DisplayMode mainDisplay;

int screenNumber = 1;

struct Image {
	SDL_Texture* t;
	SDL_Rect s;
};

vector<Image> images;


int scanPictureDir(string pathRoot);
int imageLoop();
void Center_image(SDL_Rect &size, SDL_DisplayMode &d, SDL_Surface *s);
std::wstring s2ws(const std::string& s);



int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) {
	SDL_Thread *imageLoopThread;
	SDL_Thread *scanDirLoopThread;


	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cerr << "Virhe: %s\n" << SDL_GetError() << endl;
		exit(1);
	}
	int should_be_zero = SDL_GetCurrentDisplayMode(1, &display);

	if (should_be_zero != 0) {
		// In case of error...
		SDL_Log("Could not get display mode for video display #%d: %s", 1, SDL_GetError());
		exit(1);
	}
	should_be_zero = SDL_GetCurrentDisplayMode(0, &mainDisplay);

	if (should_be_zero != 0) {
		// In case of error...
		SDL_Log("Could not get display mode for video display #%d: %s", 0, SDL_GetError());
		exit(1);
	}
	/*if (SDL_CreateWindowAndRenderer(display.w+1, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &ikkuna, &piirtaja) != 0) {
	fprintf(stderr, "Virhe: %s\n", SDL_GetError());
	exit(2);
	}*/
	mainWindow = SDL_CreateWindow("Ilmoitustaulu", mainDisplay.w + 1, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED);
	mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
	//SDL_SetWindowSize(ikkuna, 1000, 1000);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_RenderSetLogicalSize(mainRenderer, display.w, display.h);
	scanPictureDir("kuvat\\");
	//images.push_back(Load_img_to_texture(mainRenderer, "testi.png"));
	//scanDirLoopThread = SDL_CreateThread((SDL_ThreadFunction)scanPictureDir, "dir", (void *)NULL);
	//Slide slide;
	//slide.Load_image(mainRenderer, "testi.png");
	imageLoopThread = SDL_CreateThread((SDL_ThreadFunction)imageLoop, "image", (void*)NULL);

	SDL_Event event;
	//Main control loop
	while (SDL_WaitEvent(&event) >= 0) {
		switch (event.type) {


		case SDL_QUIT: {
			printf("Quit requested, quitting.\n");
			exit(0);
		}
		}
	}

	return 0;
}

int imageLoop() {
	while (1) {
		for (vector<Image>::iterator it = images.begin(); it != images.end(); it++) {
			SDL_Rect size = { 0, 0, display.w, display.h };
			SDL_RenderCopy(mainRenderer, it->t, NULL, &it->s);
			SDL_RenderPresent(mainRenderer);
			SDL_Delay(5000);
			SDL_RenderClear(mainRenderer);
			SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0);
			SDL_RenderFillRect(mainRenderer, 0);
		}
	}
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int scanPictureDir(string pathRoot) {
	//OutputDebugStringA(pathRoot.c_str());
	WIN32_FIND_DATA file;
	SDL_Surface *kuva;
	string pathRootStar = pathRoot + "*";
	HANDLE search_handle = FindFirstFile(s2ws(pathRootStar).c_str(), &file);
	bool puuttuu;
	if (search_handle)
	{
		FindNextFile(search_handle, &file);
		FindNextFile(search_handle, &file);
		do
		{
			char ch[260];
			char DefChar = ' ';

			WideCharToMultiByte(CP_ACP, 0, file.cFileName, -1, ch, 260, &DefChar, NULL);
			string path = pathRoot;
			path += ch;
			//OutputDebugStringA(ch);
			//kuva = IMG_LoadPNG_RW(SDL_RWFromFi<le(path.c_str(), "rb"));
			SDL_Surface *imgSurf = IMG_Load(path.c_str());
			if (imgSurf != NULL) {
				Image image;
				Center_image(image.s, display, imgSurf);
				image.t = SDL_CreateTextureFromSurface(mainRenderer, imgSurf);
				images.push_back(image);
			}
			SDL_FreeSurface(imgSurf);
		} while (FindNextFile(search_handle, &file));


	}
	//SDL_Delay(100);
	return 0;
}

void Center_image(SDL_Rect &size, SDL_DisplayMode &d, SDL_Surface *s) {
	float ratio = (float)s->w / (float)s->h;
	// Jos halutaan leveys h*ratio
	// Jos halutaan korkeus w/ratio
	if (d.w / ratio <= d.h) {
		size.w = d.w;
		size.h = d.w / ratio;
		size.y = (d.h - d.w / ratio) / 2;
		size.x = 0;
	}
	else {
		size.w = d.h*ratio;
		size.h = d.h;
		size.x = (d.w - d.h *ratio) / 2;
		size.y = 0;
	}
}