#include <stdio.h>	  /* printf and fprintf */
#include <SDL2/SDL.h> /* macOS- and GNU/Linux-specific */
#include <SDL_image.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct
{
	int x;
	int y;
	int w;
	int h;
	int type;
	int pressed;
	SDL_Surface *image;
} SmartPlayer_Widget;

typedef enum
{
	STATE_PLAY,
	STATE_PAUSE
} ButtonState;

const int WidgetType_Button = 0;

const int Nwidgets = 3;
const int PlayPauseAll_Button = 0;
const int Display_Button = 1;
const int ShowSideBar_Button = 2;

int showGrid = 0;
int gridSize = 175;

/*--------------------------------------------------------------------------------------------------------------------*/
void SmartPlayer_SetRect(SDL_Rect *rect, int x, int y, int w, int h)
{
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
};

/*--------------------------------------------------------------------------------------------------------------------*/
void SmartPlayer_SetRectFromWidget(SDL_Rect *rect, SmartPlayer_Widget widget)
{
	rect->x = widget.x;
	rect->y = widget.y;
	rect->w = widget.w;
	rect->h = widget.h;
};

/*--------------------------------------------------------------------------------------------------------------------*/
void SmartPlayer_CreateWidgets(SmartPlayer_Widget *widgets)
{
	widgets[PlayPauseAll_Button].w = 70;
	widgets[PlayPauseAll_Button].h = 70;
	widgets[PlayPauseAll_Button].image = IMG_Load("img/PlayPauseAll_Button_released.png");
	widgets[PlayPauseAll_Button].type = WidgetType_Button;

	widgets[Display_Button].w = 70;
	widgets[Display_Button].h = 70;
	widgets[Display_Button].image = IMG_Load("img/released.png");
	widgets[Display_Button].type = WidgetType_Button;

	widgets[ShowSideBar_Button].w = 40;
	widgets[ShowSideBar_Button].h = 40;
	widgets[ShowSideBar_Button].image = IMG_Load("img/released.png");
	widgets[ShowSideBar_Button].type = WidgetType_Button;
};

/*--------------------------------------------------------------------------------------------------------------------*/


void DisplaySlideshow(SDL_Renderer *renderer, const char *directoryPath) {
    SDL_Rect rect;
    SDL_Texture *texture;
    SDL_Surface *image;

    // Set the initial position and dimensions of the image
    rect.x = 0;
    rect.y = 0;
    rect.w = 400;
    rect.h = 400;

    while (1) {
        // Loop through image files in the directory
        for (int i = 1; ; i++) {
            char imageFilePath[256];
            sprintf(imageFilePath, "%s/%04d.png", directoryPath, i);

            image = IMG_Load(imageFilePath);
            if (!image) {
                // Image not found, break the loop
                break;
            }

            SDL_RenderClear(renderer);

            // Create a texture from the loaded image
            texture = SDL_CreateTextureFromSurface(renderer, image);
            SDL_FreeSurface(image);

            // Copy the texture to the rendering target
            SDL_RenderCopy(renderer, texture, NULL, &rect);

            // Present the rendered content to the screen
            SDL_RenderPresent(renderer);

            // Destroy the texture to free resources
            SDL_DestroyTexture(texture);

            // Pause for a short duration (adjust as needed for desired speed)

            // Process events to check for exit request
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    return;
                }
            }
        }
    }
}


/*--------------------------------------------------------------------------------------------------------------------*/


void SmartPlayer_WidgetsMouseDown(SmartPlayer_Widget *widgets, int id, ButtonState *buttonState)
{
	printf("Button pressed! %d\n", id);
	widgets[id].image = IMG_Load("img/pressed.png");

	if (id == PlayPauseAll_Button)
	{
		if (*buttonState == STATE_PLAY)
		{
			widgets[PlayPauseAll_Button].image = IMG_Load("img/PlayPauseAll_Button_pressed.png");
		}
		else
		{
			widgets[PlayPauseAll_Button].image = IMG_Load("img/playpress.png");
		}
	};
};

/*--------------------------------------------------------------------------------------------------------------------*/
void SmartPlayer_WidgetsMouseUp(SmartPlayer_Widget *widgets, int id, ButtonState *buttonState, SDL_Renderer *renderer)
{
	printf("Button released! %d\n", id);
	widgets[id].image = IMG_Load("img/released.png");
	if (id == PlayPauseAll_Button)
	{
		if (*buttonState == STATE_PLAY)
		{
			widgets[PlayPauseAll_Button].image = IMG_Load("img/playsolto.png");
			*buttonState = STATE_PAUSE;
		}
		else
		{
			widgets[PlayPauseAll_Button].image = IMG_Load("img/PlayPauseAll_Button_released.png");
			*buttonState = STATE_PLAY;
		}
	}

	if(id == Display_Button){
		showGrid = !showGrid;
	};

	if (id == ShowSideBar_Button)
	{
		char command[256];
		system("zenity --file-selection --title=Selecione --directory > selected_directory.txt");

		FILE *selectedDir = fopen("selected_directory.txt", "r");
		if (!selectedDir)
		{
			printf("Failed to open selected_directory.txt\n");
			return;
		}

		char directoryPath[256];
		fgets(directoryPath, sizeof(directoryPath), selectedDir);
		strtok(directoryPath, "\n"); // Remove the newline character

		fclose(selectedDir);

		DisplaySlideshow(renderer, directoryPath);
		// Load and display images from the selected directory
	
	};
};

/*--------------------------------------------------------------------------------------------------------------------*/
void SmartPlayer_DrawWindow(SDL_Window *window, SDL_Renderer *renderer, SmartPlayer_Widget *widgets)
{
	int i, w, h;
	int ButtonBar_height;
	SDL_Rect rect;
	SDL_Texture *texture;

	/* BackGround */
	SDL_SetRenderDrawColor(renderer, 241, 240, 240, 255); /* 7c2424 */
	SDL_GetWindowSize(window, &w, &h);
	SmartPlayer_SetRect(&rect, 0, 0, w, h);
	SDL_RenderFillRect(renderer, &rect);

	ButtonBar_height = widgets[PlayPauseAll_Button].h + 10;

	if (h > (2 * ButtonBar_height))
	{
		/* ButtonBar */
		SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
		SDL_RenderDrawLine(renderer, 0, h - ButtonBar_height, w, h - ButtonBar_height);

		/* PlayPauseAll */
		widgets[PlayPauseAll_Button].x = w / 2 - 2 - widgets[PlayPauseAll_Button].w;
		widgets[PlayPauseAll_Button].y = h - 5 - widgets[PlayPauseAll_Button].h;
		/*renderer*/
		/* Display */
		widgets[Display_Button].x = w / 2 + 2;
		widgets[Display_Button].y = h - 5 - widgets[Display_Button].h;

		/* ShowSideBar */
		widgets[ShowSideBar_Button].x = 5;
		widgets[ShowSideBar_Button].y = h - ButtonBar_height + 5;

		for (i = 0; i < Nwidgets; i++)
		{
			SmartPlayer_SetRectFromWidget(&rect, widgets[i]);
			texture = SDL_CreateTextureFromSurface(renderer, widgets[i].image);
			SDL_RenderCopy(renderer, texture, NULL, &rect);
		};

		if (showGrid)
			{
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				for (int x = 0; x < w; x += gridSize)
				{
					SDL_RenderDrawLine(renderer, x, 0, x, h);
				}
				for (int y = 0; y < h; y += gridSize)
				{
					SDL_RenderDrawLine(renderer, 0, y, w, y);
				}
			}
	};

	SDL_RenderPresent(renderer);
};

/*--------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	ButtonState buttonState = STATE_PLAY;
	SmartPlayer_Widget widgets[Nwidgets];

	int i;

	/* Inicializa a SDL */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
		return 1;
	}
	IMG_Init(IMG_INIT_JPG);

	/* Cria as janelas */
	window = SDL_CreateWindow("SmartPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_RESIZABLE);
	if (window == NULL)
	{
		fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SmartPlayer_CreateWidgets(widgets);
int windowWidth, windowHeight;
SDL_GetWindowSize(window, &windowWidth, &windowHeight);

	/* Loop Principal */
	i = 0;
	while (1)
	{
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				break;
			};

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					for (i = 0; i < Nwidgets; i++)
					{
						if (widgets[i].type == WidgetType_Button)
						{
							if ((event.button.x > widgets[i].x) && (event.button.x < (widgets[i].x + widgets[i].w)))
							{
								if ((event.button.y > widgets[i].y) && (event.button.y < (widgets[i].y + widgets[i].h)))
								{
									if (widgets[i].pressed == FALSE)
										SmartPlayer_WidgetsMouseDown(widgets, i, &buttonState);
									widgets[i].pressed = TRUE;
								};
							};
						};
					};
				};
			};

			if (event.type == SDL_MOUSEBUTTONUP)
			{
				for (i = 0; i < Nwidgets; i++)
				{
					if (widgets[i].type == WidgetType_Button)
					{
						if (widgets[i].pressed == TRUE)
							SmartPlayer_WidgetsMouseUp(widgets, i, &buttonState,renderer);
						widgets[i].pressed = FALSE;
					};
				};
			};

			if (event.type == SDL_MOUSEMOTION)
			{
				for (i = 0; i < Nwidgets; i++)
				{
					if (widgets[i].type == WidgetType_Button)
					{
						if ((event.button.x < widgets[i].x) || (event.button.x > (widgets[i].x + widgets[i].w)))
						{
							if (widgets[i].pressed == TRUE)
								SmartPlayer_WidgetsMouseUp(widgets, i, &buttonState,renderer);
							widgets[i].pressed = FALSE;
						};
						if ((event.button.y < widgets[i].y) || (event.button.y > (widgets[i].y + widgets[i].h)))
						{
							if (widgets[i].pressed == TRUE)
								SmartPlayer_WidgetsMouseUp(widgets, i, &buttonState,renderer);
							widgets[i].pressed = FALSE;
						};
					};
				};
			};

			

			
		};

		if (i > 100)
		{
			SmartPlayer_DrawWindow(window, renderer, widgets);
			i = 0;
		};

		SDL_Delay(1);
		i++;
	};
	
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}
