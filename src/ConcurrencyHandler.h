#pragma once

#include <QObject>
#include <QtConcurrent>

#include "RayTracer.h"

class ConcurrencyHandler : public QObject
{
	Q_OBJECT

public:

	explicit ConcurrencyHandler(QObject* parent = nullptr, RayTracer* rayTracer = nullptr);

	ConcurrencyHandler(const ConcurrencyHandler& other) = default;

	ConcurrencyHandler(ConcurrencyHandler&& other) = default;

	ConcurrencyHandler& operator=(const ConcurrencyHandler& other) = default;

	ConcurrencyHandler& operator=(ConcurrencyHandler&& other) = default;

	~ConcurrencyHandler() override = default;

	void init(RayTracer* rayTracer);

	bool isRendering() const;

public slots:

	void onRender();

	void onRenderPreview();

	void onCancelRendering();

private slots:

	void onRenderFinished();

signals:

	void renderFinished(int microseconds);

	void renderPreviewFinished();

private:

	RayTracer* m_rayTracer = nullptr;

	QFuture<int> m_renderFuture;
	QFutureWatcher<int> m_renderFutureWatcher;

	void connect();

	void render();

	void renderPreview();

	bool m_connected = false;
	bool m_previewAndFinal = false;
};

