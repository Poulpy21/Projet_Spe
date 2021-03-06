#include "headers.hpp"
#include "model.hpp"
#include "model.moc"
#include "openGLScene.hpp"
#include "initialCondFactory.hpp"


Display *Model::solverDisplay=0;
GLXContext Model::solverContext = 0;
Window Model::solverWindow=0;
Colormap Model::solverColormap=0;

Model::Model(unsigned int nbIter, 
		std::map<QString, bool> *renderedVars, 
		std::map<QString,int> *initialCondsId, 
		unsigned int width, unsigned int height, unsigned int length,
		bool save, const QString &directory, unsigned int saveRate):
	m_width(width), m_height(height), m_length(length), 
	m_renderedVars(renderedVars), 
	m_initialCondsId(initialCondsId), 
	m_save(save), m_saveDirectory(directory), m_saveRate(saveRate),
	m_nbIter(nbIter),
	m_pause(false), m_stop(false)
{
}
		
Model::~Model() {
	for(auto pair : m_initialConds)
		delete pair.second;
}

void Model::startComputing() {

	//Create openGL context for the solver, try to get shared context with QT (if not already done)
	if(!solverContext) {
		log_console->infoStream() << "Creating openGL context for the solver...";
		utils::createOpenGLContext(&solverDisplay, &solverContext, &solverWindow, &solverColormap, OpenGLScene::qtContext);
	}
		
	//print infos about current openGL context
	int glxConfigXid=0, glxSupportedRenderType=0, glxScreenNumber=0;
	glXQueryContext(Model::solverDisplay, Model::solverContext,  GLX_FBCONFIG_ID, &glxConfigXid);
	glXQueryContext(Model::solverDisplay, Model::solverContext, GLX_RENDER_TYPE, &glxSupportedRenderType);
	glXQueryContext(Model::solverDisplay, Model::solverContext, GLX_SCREEN, &glxScreenNumber);

	log_console->infoStream() << "Solver openGL context : "
		<< " XID=" << glxConfigXid
		<< " RenderType=" << glxSupportedRenderType
		<< " ScreenNumber=" << glxScreenNumber;

	//call factory for initial conditions
	InitialCondFactory *factory = new InitialCondFactory();
	connect(factory, SIGNAL(showCodeEditor(int)), this, SIGNAL(showCodeEditor(int)));
	connect(this, SIGNAL(codeSubmitted(const QString &)), factory, SLOT(codeSubmitted(const QString &)), Qt::DirectConnection);
	for(auto pair : *m_initialCondsId)
		m_initialConds.emplace(pair.first, factory->getInitialCond(pair.second));
	
	log_console->infoStream() << "Start Computing.";
    initComputation();

    QElapsedTimer screenRefreshTimer;
    screenRefreshTimer.start();
	

    for (m_stepId = 0; m_stepId < m_nbIter; m_stepId++) {
        m_mutex.lock();
        while (m_pause) {
            m_cond.wait(&m_mutex);
        }
        bool stop = m_stop;
        m_cond.wakeOne();
        m_mutex.unlock();
        
        if (stop) {
            break;
        }

        computeStep(m_stepId);
        
		// Update screen only after at least 17 ms (~60 fps)
        if (screenRefreshTimer.hasExpired(17)) {
            emit updateDisplay(m_mappedTexturesGui);
            screenRefreshTimer.restart();
        }

		if(m_save && m_stepId%m_saveRate==0) {
			for(auto pair : *m_renderedVars) {
				if(pair.second)
					saveGrid(pair.first.toStdString(), "grid_", ".data");
			}
		}
		
		emit stepComputed();
    }
    
    finishComputation();
    emit finished();
}

void Model::pauseComputing(bool b) {
    m_mutex.lock();
    m_pause = b;
    if (!b)
        m_cond.wakeAll();
    m_mutex.unlock();
}

void Model::stopComputing() {
    m_mutex.lock();
    m_stop = true;
    m_cond.wakeAll();
    m_mutex.unlock();
}
		
void Model::saveGrid(std::string varName, std::string prefix, std::string suffix) {
	std::stringstream ss;
	ss << m_saveDirectory.toStdString() << "/" << prefix
			<< varName << "_" << m_stepId << suffix;
	if(m_gridsToSave[varName])
		log_console->debugStream() << "WRITE" << ss.str();
		m_gridsToSave[varName]->exportGrid(ss.str());
}
