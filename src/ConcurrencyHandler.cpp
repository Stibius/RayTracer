#include "ConcurrencyHandler.h"

ConcurrencyHandler::ConcurrencyHandler(QObject* parent, RayTracer* rayTracer)
	: QObject(parent)
	, m_rayTracer(rayTracer)
{

}

void ConcurrencyHandler::init(RayTracer* rayTracer)
{
	onCancelRendering();

	m_rayTracer = rayTracer;
}

bool ConcurrencyHandler::isRendering() const
{
	return m_renderFuture.isRunning();
}

void ConcurrencyHandler::onRender()
{
	if (m_connected == false)
	{
		connect();
	}

	onCancelRendering();

	m_rayTracer->setPreviewMode(true);

	render();
}

void ConcurrencyHandler::onRenderPreview()
{
	if (m_connected == false)
	{
		connect();
	}

	onCancelRendering();

	m_rayTracer->setPreviewMode(true);

	renderPreview();
}

void ConcurrencyHandler::onCancelRendering()
{
	if (m_renderFuture.isRunning())
	{
		m_rayTracer->cancelRendering();
		m_renderFuture.waitForFinished();
	}
}

void ConcurrencyHandler::onRenderFinished()
{
	if (m_rayTracer->getPreviewMode() == true)
	{
		emit renderPreviewFinished();
	}
	else
	{
		emit renderFinished(m_renderFuture.result());
	}

	if (m_previewAndFinal == true && m_rayTracer->getPreviewMode() == true)
	{
		m_rayTracer->setPreviewMode(false);

		render();
	}
}

void ConcurrencyHandler::connect()
{
	QObject::connect(&m_renderFutureWatcher, &QFutureWatcher<void>::finished, this, &ConcurrencyHandler::onRenderFinished);

	m_connected = true;
}

void ConcurrencyHandler::render()
{
	m_previewAndFinal = true;

	m_renderFuture = QtConcurrent::run([this]()
	{
		return m_rayTracer->render();
	});

	m_renderFutureWatcher.setFuture(m_renderFuture);
}

void ConcurrencyHandler::renderPreview()
{
	m_previewAndFinal = false;

	m_renderFuture = QtConcurrent::run([this]()
	{
		return m_rayTracer->render();
	});

	m_renderFutureWatcher.setFuture(m_renderFuture);
}
