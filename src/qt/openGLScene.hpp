#ifndef OPENGL_SCENE_HPP
#define OPENGL_SCENE_HPP

#include "headers.hpp"
#include <mutex>
#include <condition_variable>
#include "program.hpp"
#include "texture.hpp"
#include "graphicsViewer.hpp"
#include "grid2D.hpp"


class OpenGLScene : public QGraphicsScene {
    Q_OBJECT

    public:
        OpenGLScene(GraphicsViewer *viewer);
		~OpenGLScene();

        void drawBackground(QPainter *painter, const QRectF &);
		
		static Display *qtDisplay;
		static GLXContext qtContext;
		
    public slots:
        void updateTextures(const QMap<QString, GLuint> &texMap);
		void changeColormap(const QString &colormapName);
		void toggleSampler();

    private:
        GraphicsViewer *m_viewer;
		Program *m_drawProgram;
		std::map<std::string, int> m_drawProgramUniformLocationMap;
		Texture *m_currentTexture;
		unsigned int m_texCoordsVBO;
		unsigned int m_vertexCoordsVBO;
		unsigned int m_texture;
        QMap<QString, GLuint> m_texMap;
        int m_nTexturesWidth;
        int m_nTexturesHeight;
		unsigned int m_colormapsUBO;
		unsigned int m_colorId;
        float *m_vertexCoords;
		unsigned int m_min_mag_filter;
		
		void makeProgram();
		void makeColorMaps();
		void makeArrays();
	
		void renderString(float x, float y, void *font, const char* string,
				float r=0.0f, float g=0.0f, float b=1.0f, float a=1.0f);

};
#endif

