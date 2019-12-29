#include "SettingsWindow.h"
#include "Application.h"
#include "SettingsIO.h"
#include "AssimpModelLoader.h"
#include "PerspectiveCamera.h"
#include "OrthoCamera.h"
#include "Utility.h"
#include "Quadric.h"
#include "CompositeShape.h"
#include "ui_SettingsWindow.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>

const QString SettingsWindow::DEFAULT_LIGHT_NAME = "Light";
const QString SettingsWindow::DEFAULT_MATERIAL_NAME = "Material";
const QString SettingsWindow::DEFAULT_SHAPE_NAME = "Object";

SettingsWindow::SettingsWindow(QWidget* parent) 
	: QMainWindow(parent)
	, ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

	ui->lvLights->setModel(&m_lightListModel);
	ui->lvMaterials->setModel(&m_materialListModel);
	ui->cbMat->setModel(&m_materialComboBoxModel);
	ui->tvShapes->setModel(&m_shapeTreeModel);

	m_lightListModel.setColumnCount(1);
	m_materialListModel.setColumnCount(1);
	m_materialComboBoxModel.setColumnCount(1);
	m_shapeTreeModel.setColumnCount(1);

	connect(ui->lvLights->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SettingsWindow::on_currentLightChanged);
	connect(ui->lvMaterials->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SettingsWindow::on_currentMaterialChanged);
	connect(ui->tvShapes->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &SettingsWindow::on_currentShapeChanged);
	connect(ui->tvShapes->model(), &QAbstractItemModel::dataChanged, this, &SettingsWindow::on_tvShapesDataChanged);
	connect(ui->lvMaterials->model(), &QAbstractItemModel::dataChanged, this, &SettingsWindow::on_lvMaterialsDataChanged);
	connect(ui->lvLights->model(), &QAbstractItemModel::dataChanged, this, &SettingsWindow::on_lvLightsDataChanged);

	loadSettings();
	
	ui->btnCopyLight->setEnabled(false);
	ui->btnCopyMat->setEnabled(false);
	ui->btnCopyShape->setEnabled(false);
	ui->btnDeleteLight->setEnabled(false);
	ui->btnDeleteMat->setEnabled(false);
	ui->btnDeleteShape->setEnabled(false);

	QSizePolicy sp_retain = ui->itemDataTabWidget->sizePolicy();
	sp_retain.setRetainSizeWhenHidden(true);
	ui->itemDataTabWidget->setSizePolicy(sp_retain);
	ui->itemDataTabWidget->tabBar()->hide();
	ui->itemDataTabWidget->setVisible(false);
	ui->materialDataTabWidget->tabBar()->hide();
	ui->shapeDataTabWidget->tabBar()->hide();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_cameraPosChanged()
{
	loadCameraSettings();
}

void SettingsWindow::loadSettings()
{
	loadRayTracerSettings();

	loadCameraSettings();

	loadLightItems();

	loadMaterialItems();

	loadShapeItems();
}

void SettingsWindow::on_currentLightChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (QModelIndex index = ui->lvLights->currentIndex(); !index.isValid())
	{
		ui->btnCopyLight->setEnabled(false);
		ui->btnDeleteLight->setEnabled(false);
		ui->itemDataTabWidget->setVisible(false);
	}
	else
	{
		ui->lvMaterials->setCurrentIndex(QModelIndex());
		ui->tvShapes->setCurrentIndex(QModelIndex());

		ui->btnCopyLight->setEnabled(true);
		ui->btnDeleteLight->setEnabled(true);
		
		loadSelectedLightSettings();
	}
}

void SettingsWindow::on_btnDeleteLight_clicked()
{
	QModelIndex index = ui->lvLights->currentIndex();
	Light* light = static_cast<Light*>(index.data(Qt::UserRole).value<void*>());

	Application::m_scene.eraseLight(light);
	loadLightItems();
}

void SettingsWindow::on_lvLightsDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	if (!roles.contains(Qt::DisplayRole)) return;

	QString newName = topLeft.data(Qt::DisplayRole).toString().trimmed();

	if (checkNameValidity(newName, ui->lvLights, topLeft))
	{
		Light* light = static_cast<Light*>(topLeft.data(Qt::UserRole).value<void*>());
		light->setName(newName.toStdString());
		loadSelectedLightSettings();
	}
	else
	{
		loadLightItems();
	}
}

void SettingsWindow::on_btnAddLight_clicked()
{
	ui->lvMaterials->setCurrentIndex(QModelIndex());
	ui->tvShapes->setCurrentIndex(QModelIndex());

	QString name = createUniqueItemName(ui->lvLights, DEFAULT_LIGHT_NAME);
	Light* light = Application::m_scene.addLight(PointLight(Vector(0.0, 0.0, 10.0), Color(1.0, 1.0, 1.0), name.toStdString()));
	
	addLightItem(light);

	QModelIndex newIndex = m_lightListModel.index(m_lightListModel.rowCount() - 1, 0);
	ui->lvLights->setCurrentIndex(newIndex);
}

void SettingsWindow::loadSelectedLightSettings()
{
	QModelIndex index = ui->lvLights->currentIndex();

	Light* light = static_cast<Light*>(index.data(Qt::UserRole).value<void*>());

	ui->chkLightEnabled->setChecked(light->isEnabled());
	ui->lnLightName->setText(QString::fromStdString(light->getName()));
	ui->lnLightName->setStyleSheet("color : #000000");
	ui->lnLightName->setCursorPosition(0);
	QColor color = QColor(Utility::colorToQColor(light->getColor()));
	ui->btnLightColor->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	const Vector& position = light->getPosition();
	ui->sbLightXPos->setValue(position.m_x);
	ui->sbLightYPos->setValue(position.m_y);
	ui->sbLightZPos->setValue(position.m_z);

	if (PointLight* pointLight = dynamic_cast<PointLight*>(light))
	{
		ui->lbRadius->setVisible(false);
		ui->sbLightRadius->setVisible(false);
		ui->cbLightType->setCurrentText("Point light");
	}
	else if (SphereLight* sphereLight = dynamic_cast<SphereLight*>(light))
	{
		ui->lbRadius->setVisible(true);
		ui->sbLightRadius->setVisible(true);
		ui->sbLightRadius->setValue(sphereLight->getRadius());
		ui->cbLightType->setCurrentText("Sphere light");
	}

	ui->itemDataTabWidget->setVisible(true);
	ui->itemDataTabWidget->setCurrentIndex(0);
}

void SettingsWindow::loadSelectedMaterialSettings()
{
	QModelIndex index = ui->lvMaterials->currentIndex();

	Material* material = static_cast<Material*>(index.data(Qt::UserRole).value<void*>());

	if (SimpleMaterial* simpleMat = dynamic_cast<SimpleMaterial*>(material))
	{
		ui->materialDataTabWidget->setCurrentIndex(0);

		const MaterialProperties& materialProperties = simpleMat->getProperties();

		QColor diffColor = Utility::colorToQColor(materialProperties.m_diffuse);
		ui->btnMatColorDiffuse->setStyleSheet("border: 1px solid black; background-color: " + diffColor.name());
		QColor specColor = Utility::colorToQColor(materialProperties.m_specular);
		ui->btnMatColorSpecular->setStyleSheet("border: 1px solid black; background-color: " + specColor.name());
		QColor ambColor = Utility::colorToQColor(materialProperties.m_ambient);
		ui->btnMatColorAmbient->setStyleSheet("border: 1px solid black; background-color: " + ambColor.name());
		ui->sbMatShininess->setValue(materialProperties.m_shininess);
		ui->sbMatReflectance->setValue(materialProperties.m_reflectance);
		ui->sbMatReflectDistAngle->setValue(materialProperties.m_reflectionDistAngle);
		ui->sbMatTransparency->setValue(materialProperties.m_transparency);
		ui->sbMatRefIndex->setValue(materialProperties.m_refractionIndex);
		ui->sbMatRefractDistAngle->setValue(materialProperties.m_refractionDistAngle);

		ui->sbCheckerTileX->setValue(1.0);
		ui->sbCheckerTileY->setValue(1.0);
		ui->sbCheckerTileZ->setValue(1.0);

		ui->btnMatColorDiffuse_2->setStyleSheet("border: 1px solid black; background-color: #000000");
		ui->btnMatColorSpecular_2->setStyleSheet("border: 1px solid black; background-color: #ffffff");
		ui->btnMatColorAmbient_2->setStyleSheet("border: 1px solid black; background-color: #000000");
		ui->sbMatShininess_2->setValue(15);
		ui->sbMatReflectance_2->setValue(0.0);
		ui->sbMatReflectDistAngle_2->setValue(0.0);
		ui->sbMatTransparency_2->setValue(0.0);
		ui->sbMatRefIndex_2->setValue(1.0);
		ui->sbMatRefractDistAngle_2->setValue(0.0);

		ui->btnMatColorDiffuse_3->setStyleSheet("border: 1px solid black; background-color: #ffffff");
		ui->btnMatColorSpecular_3->setStyleSheet("border: 1px solid black; background-color: #ffffff");
		ui->btnMatColorAmbient_3->setStyleSheet("border: 1px solid black; background-color: #404040");
		ui->sbMatShininess_3->setValue(15);
		ui->sbMatReflectance_3->setValue(0.0);
		ui->sbMatReflectDistAngle_3->setValue(0.0);
		ui->sbMatTransparency_3->setValue(0.0);
		ui->sbMatRefIndex_3->setValue(1.0);
		ui->sbMatRefractDistAngle_3->setValue(0.0);

		ui->lnMatName->setText(QString::fromStdString(material->getName()));
		ui->lnMatName->setStyleSheet("color : #000000");
		ui->lnMatName->setCursorPosition(0);

		ui->cbMatType->setCurrentText("Simple");
	}
	else if (CheckerMaterial* checkMat = dynamic_cast<CheckerMaterial*>(material))
	{
		ui->materialDataTabWidget->setCurrentIndex(1);

		ui->btnMatColorDiffuse->setStyleSheet("border: 1px solid black; background-color: #ff804f");
		ui->btnMatColorSpecular->setStyleSheet("border: 1px solid black; background-color: #808080");
		ui->btnMatColorAmbient->setStyleSheet("border: 1px solid black; background-color: #1a0d08");
		ui->sbMatShininess->setValue(32);
		ui->sbMatReflectance->setValue(0.0);
		ui->sbMatReflectDistAngle->setValue(0.0);
		ui->sbMatTransparency->setValue(0.0);
		ui->sbMatRefIndex->setValue(1.0);
		ui->sbMatRefractDistAngle->setValue(0.0);

		const Vector& tileSize = checkMat->getTileSize();

		ui->sbCheckerTileX->setValue(tileSize.m_x);
		ui->sbCheckerTileY->setValue(tileSize.m_y);
		ui->sbCheckerTileZ->setValue(tileSize.m_z);

		const MaterialProperties& materialProperties1 = checkMat->getProperties1();

		QColor diffColor = Utility::colorToQColor(materialProperties1.m_diffuse);
		ui->btnMatColorDiffuse_2->setStyleSheet("border: 1px solid black; background-color: " + diffColor.name());
		QColor specColor = Utility::colorToQColor(materialProperties1.m_specular);
		ui->btnMatColorSpecular_2->setStyleSheet("border: 1px solid black; background-color: " + specColor.name());
		QColor ambColor = Utility::colorToQColor(materialProperties1.m_ambient);
		ui->btnMatColorAmbient_2->setStyleSheet("border: 1px solid black; background-color: " + ambColor.name());
		ui->sbMatShininess_2->setValue(materialProperties1.m_shininess);
		ui->sbMatReflectance_2->setValue(materialProperties1.m_reflectance);
		ui->sbMatReflectDistAngle_2->setValue(materialProperties1.m_reflectionDistAngle);
		ui->sbMatTransparency_2->setValue(materialProperties1.m_transparency);
		ui->sbMatRefIndex_2->setValue(materialProperties1.m_refractionIndex);
		ui->sbMatRefractDistAngle_2->setValue(materialProperties1.m_refractionDistAngle);

		const MaterialProperties& materialProperties2 = checkMat->getProperties2();

		QColor diffColor2 = Utility::colorToQColor(materialProperties2.m_diffuse);
		ui->btnMatColorDiffuse_3->setStyleSheet("border: 1px solid black; background-color: " + diffColor2.name());
		QColor specColor2 = Utility::colorToQColor(materialProperties2.m_specular);
		ui->btnMatColorSpecular_3->setStyleSheet("border: 1px solid black; background-color: " + specColor2.name());
		QColor ambColor2 = Utility::colorToQColor(materialProperties2.m_ambient);
		ui->btnMatColorAmbient_3->setStyleSheet("border: 1px solid black; background-color: " + ambColor2.name());
		ui->sbMatShininess_3->setValue(materialProperties2.m_shininess);
		ui->sbMatReflectance_3->setValue(materialProperties2.m_reflectance);
		ui->sbMatReflectDistAngle_3->setValue(materialProperties2.m_reflectionDistAngle);
		ui->sbMatTransparency_3->setValue(materialProperties2.m_transparency);
		ui->sbMatRefIndex_3->setValue(materialProperties2.m_refractionIndex);
		ui->sbMatRefractDistAngle_3->setValue(materialProperties2.m_refractionDistAngle);

		ui->lnMatName->setText(QString::fromStdString(material->getName()));
		ui->lnMatName->setStyleSheet("color : #000000");
		ui->lnMatName->setCursorPosition(0);

		ui->cbMatType->setCurrentText("Checkerboard");
	}

	ui->itemDataTabWidget->setVisible(true);
	ui->itemDataTabWidget->setCurrentIndex(1);
}

void SettingsWindow::loadSelectedShapeSettings()
{
	QModelIndex index = ui->tvShapes->currentIndex();

	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());

	if (Quadric* quadric = dynamic_cast<Quadric*>(shape))
	{
		ui->shapeDataTabWidget->setCurrentIndex(0);

		ui->sbA->setValue(quadric->getA());
		ui->sbB->setValue(quadric->getB());
		ui->sbC->setValue(quadric->getC());
		ui->sbD->setValue(quadric->getD());
		ui->sbE->setValue(quadric->getE());
		ui->sbF->setValue(quadric->getF());
		ui->sbG->setValue(quadric->getG());
		ui->sbH->setValue(quadric->getH());
		ui->sbI->setValue(quadric->getI());
		ui->sbJ->setValue(quadric->getJ());

		ui->cbShapeType->setCurrentText("Quadric");
	}
	else if (Plane* plane = dynamic_cast<Plane*>(shape))
	{
		ui->shapeDataTabWidget->setCurrentIndex(1);

		ui->sbPlaneOriginX->setValue(plane->getOrigin().m_x);
		ui->sbPlaneOriginY->setValue(plane->getOrigin().m_y);
		ui->sbPlaneOriginZ->setValue(plane->getOrigin().m_z);
		ui->sbPlaneNormalX->setValue(plane->getNormal().m_x);
		ui->sbPlaneNormalY->setValue(plane->getNormal().m_y);
		ui->sbPlaneNormalZ->setValue(plane->getNormal().m_z);

		ui->cbShapeType->setCurrentText("Plane");
	}
	else if (Model* model = dynamic_cast<Model*>(shape))
	{
		ui->shapeDataTabWidget->setCurrentIndex(2);

		if (model->isEmpty())
		{
			ui->modelStatusLabel->setText("empty");
		}
		else
		{
			ui->modelStatusLabel->setText("loaded");
		}
		
		ui->chkSmooth->setChecked(model->isSmooth());
		ui->cbShapeType->setCurrentText("Model");
	}
	else if (CompositeShape* compositeShape = dynamic_cast<CompositeShape*>(shape))
	{
		ui->shapeDataTabWidget->setCurrentIndex(3);

		ui->lnCompositeExpression->setText(QString::fromStdString(compositeShape->getExpression()));
		ui->lnCompositeExpression->setStyleSheet("color : #000000");
		ui->lnCompositeExpression->setCursorPosition(0);
		ui->lnCompositeExpression->setEnabled(!compositeShape->hasChildren());

		ui->btnBreakComposite->setEnabled(compositeShape->hasChildren());

		if (checkCompositeExpressionValidity(ui->lnCompositeExpression->text().trimmed(), index) == true)
		{
			ui->btnCreateComposite->setEnabled(true);
		}
		else
		{
			ui->btnCreateComposite->setEnabled(false);
		}

		ui->cbShapeType->setCurrentText("Composite object");
	}

	ui->chkShapeEnabled->setChecked(shape->isEnabled());
	ui->chkShapeEnabled->setEnabled(!index.parent().isValid());
	ui->lnShapeName->setText(QString::fromStdString(shape->getName()));
	ui->lnShapeName->setCursorPosition(0);

	const Material* shapeMaterial = shape->getMaterial();
	for (int i = 0; i < m_materialComboBoxModel.rowCount(); ++i)
	{
		QModelIndex index = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(index.data(Qt::UserRole).value<void*>());
		if (shapeMaterial == material)
		{
			ui->cbMat->setCurrentIndex(i);
			break;
		}
	}

	ui->itemDataTabWidget->setVisible(true);
	ui->itemDataTabWidget->setCurrentIndex(2);
}

void SettingsWindow::saveRayTracerSettings()
{
	Application::m_rayTracer.setBackgroundColor(Utility::styleSheetToColor(ui->btnBackColor->styleSheet()));
	Application::m_rayTracer.setAdaptiveSuperSampling(ui->chkSuper->isChecked());
	Application::m_rayTracer.setSuperSamplingThreshold(ui->sbTreshold->value());
	Application::m_rayTracer.setSubSamplingSize(ui->sbSubsampling->value());
	Application::m_rayTracer.setRecursion(ui->sbRecursion->value());
	Application::m_rayTracer.setShadowDist(ui->sbShadowCount->value());
	Application::m_rayTracer.setReflectionDist(ui->sbReflectionCount->value());
	Application::m_rayTracer.setRefractionDist(ui->sbRefractionCount->value());
}

void SettingsWindow::saveCameraSettings()
{
	const Vector& position = Application::m_scene.getCamera()->getPosition();
	const Vector& direction = Application::m_scene.getCamera()->getDirection();
	const Vector& right = Application::m_scene.getCamera()->getRight();
	const Vector& up = Application::m_scene.getCamera()->getUp();
	if (ui->rbPerspective->isChecked())
	{
		PerspectiveCamera camera;
		camera.setFOV(ui->sbFOV->value());
		Application::m_scene.setCamera(camera);
	}
	else if (ui->rbOrtho->isChecked())
	{
		OrthoCamera camera;
		camera.setOrthoWidth(ui->sbOrthoWidth->value());
		camera.setOrthoHeight(ui->sbOrthoHeight->value());
		Application::m_scene.setCamera(camera);
	}
	Camera* camera = Application::m_scene.getCamera();
	camera->setPosition(position);
	camera->setDirection(direction);
	camera->setRight(right);
	camera->setUp(up);
	camera->setRotationCoef(ui->sbCamRotSpeed->value());
	camera->setStepSize(ui->sbCameraStepSize->value());
	camera->setPosition(Vector(ui->sbCamX->value(), ui->sbCamY->value(), ui->sbCamZ->value()));
	Application::m_rayTracer.init(&Application::m_scene, camera);
	Application::m_keyEventHandler.init(camera);
	Application::m_mouseEventHandler.init(camera);
}

void SettingsWindow::saveSelectedLightSettings()
{
	QModelIndex index = ui->lvLights->currentIndex();

	if (ui->cbLightType->currentText() == "Sphere light")
	{
		SphereLight* light = static_cast<SphereLight*>(index.data(Qt::UserRole).value<void*>());
		light->setEnabled(ui->chkLightEnabled->isChecked());
		light->setPosition(Vector(ui->sbLightXPos->value(), ui->sbLightYPos->value(), ui->sbLightZPos->value()));
		light->setColor(Utility::styleSheetToColor(ui->btnLightColor->styleSheet()));
		light->setName(ui->lnLightName->text().trimmed().toStdString());
		light->setRadius(ui->sbLightRadius->value());
	}
	else if (ui->cbLightType->currentText() == "Point light")
	{
		PointLight* light = static_cast<PointLight*>(index.data(Qt::UserRole).value<void*>());
		light->setEnabled(ui->chkLightEnabled->isChecked());
		light->setPosition(Vector(ui->sbLightXPos->value(), ui->sbLightYPos->value(), ui->sbLightZPos->value()));
		light->setColor(Utility::styleSheetToColor(ui->btnLightColor->styleSheet()));
		light->setName(ui->lnLightName->text().trimmed().toStdString());
	}
}

void SettingsWindow::saveSelectedMaterialSettings()
{
	QModelIndex index = ui->lvMaterials->currentIndex();

	if (ui->cbMatType->currentText() == "Simple")
	{
		SimpleMaterial* material = static_cast<SimpleMaterial*>(index.data(Qt::UserRole).value<void*>());

		MaterialProperties materialProperties;

		materialProperties.m_diffuse = Color(Utility::styleSheetToColor(ui->btnMatColorDiffuse->styleSheet()));
		materialProperties.m_specular = Color(Utility::styleSheetToColor(ui->btnMatColorSpecular->styleSheet()));
		materialProperties.m_ambient = Color(Utility::styleSheetToColor(ui->btnMatColorAmbient->styleSheet()));
		materialProperties.m_shininess = ui->sbMatShininess->value();
		materialProperties.m_reflectance = ui->sbMatReflectance->value();
		materialProperties.m_reflectionDistAngle = ui->sbMatReflectDistAngle->value();
		materialProperties.m_transparency = ui->sbMatTransparency->value();
		materialProperties.m_refractionIndex = ui->sbMatRefIndex->value();
		materialProperties.m_refractionDistAngle = ui->sbMatRefractDistAngle->value();

		material->setProperties(materialProperties);
		material->setName(ui->lnMatName->text().toStdString());
	}
	else if (ui->cbMatType->currentText() == "Checkerboard")
	{
		CheckerMaterial* material = static_cast<CheckerMaterial*>(index.data(Qt::UserRole).value<void*>());

		MaterialProperties materialProperties1;

		materialProperties1.m_diffuse = Color(Utility::styleSheetToColor(ui->btnMatColorDiffuse_2->styleSheet()));
		materialProperties1.m_specular = Color(Utility::styleSheetToColor(ui->btnMatColorSpecular_2->styleSheet()));
		materialProperties1.m_ambient = Color(Utility::styleSheetToColor(ui->btnMatColorAmbient_2->styleSheet()));
		materialProperties1.m_shininess = ui->sbMatShininess_2->value();
		materialProperties1.m_reflectance = ui->sbMatReflectance_2->value();
		materialProperties1.m_reflectionDistAngle = ui->sbMatReflectDistAngle_2->value();
		materialProperties1.m_transparency = ui->sbMatTransparency_2->value();
		materialProperties1.m_refractionIndex = ui->sbMatRefIndex_2->value();
		materialProperties1.m_refractionDistAngle = ui->sbMatRefractDistAngle_2->value();

		MaterialProperties materialProperties2;

		materialProperties2.m_diffuse = Color(Utility::styleSheetToColor(ui->btnMatColorDiffuse_3->styleSheet()));
		materialProperties2.m_specular = Color(Utility::styleSheetToColor(ui->btnMatColorSpecular_3->styleSheet()));
		materialProperties2.m_ambient = Color(Utility::styleSheetToColor(ui->btnMatColorAmbient_3->styleSheet()));
		materialProperties2.m_shininess = ui->sbMatShininess_3->value();
		materialProperties2.m_reflectance = ui->sbMatReflectance_3->value();
		materialProperties2.m_reflectionDistAngle = ui->sbMatReflectDistAngle_3->value();
		materialProperties2.m_transparency = ui->sbMatTransparency_3->value();
		materialProperties2.m_refractionIndex = ui->sbMatRefIndex_3->value();
		materialProperties2.m_refractionDistAngle = ui->sbMatRefractDistAngle_3->value();

		material->setProperties(materialProperties1, materialProperties2);
		material->setTileSize(Vector(ui->sbCheckerTileX->value(), ui->sbCheckerTileY->value(), ui->sbCheckerTileZ->value()));
		material->setName(ui->lnMatName->text().toStdString());
	}
}

void SettingsWindow::saveSelectedShapeSettings()
{
	QModelIndex index = ui->tvShapes->currentIndex();

	if (ui->cbShapeType->currentText() == "Quadric")
	{
		Quadric* quadric = static_cast<Quadric*>(index.data(Qt::UserRole).value<void*>());

		quadric->setParameters(ui->sbA->value(), ui->sbB->value(), ui->sbC->value(),
			ui->sbD->value(), ui->sbE->value(), ui->sbF->value(),
			ui->sbG->value(), ui->sbH->value(), ui->sbI->value(),
			ui->sbJ->value());
		quadric->setEnabled(ui->chkShapeEnabled->isChecked());

		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		quadric->setMaterial(material);
		quadric->setName(ui->lnShapeName->text().trimmed().toStdString());
	}
	else if (ui->cbShapeType->currentText() == "Plane")
	{
		Plane* plane = static_cast<Plane*>(index.data(Qt::UserRole).value<void*>());

		plane->setOrigin(Vector(ui->sbPlaneOriginX->value(),
			ui->sbPlaneOriginY->value(),
			ui->sbPlaneOriginZ->value()));
		plane->setNormal(Vector(ui->sbPlaneNormalX->value(),
			ui->sbPlaneNormalY->value(),
			ui->sbPlaneNormalZ->value()));
		plane->setEnabled(ui->chkShapeEnabled->isChecked());

		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		plane->setMaterial(material);
		plane->setName(ui->lnShapeName->text().trimmed().toStdString());
	}
	else if (ui->cbShapeType->currentText() == "Model")
	{
		Model* model = static_cast<Model*>(index.data(Qt::UserRole).value<void*>());

		model->setSmooth(ui->chkSmooth->isChecked());
		model->setEnabled(ui->chkShapeEnabled->isChecked());

		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		model->setMaterial(material);
		model->setName(ui->lnShapeName->text().trimmed().toStdString());
	}
	else if (ui->cbShapeType->currentText() == "Composite object")
	{
		ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());

		shape->setEnabled(ui->chkShapeEnabled->isChecked());

		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		shape->setMaterial(material);
		shape->setName(ui->lnShapeName->text().trimmed().toStdString());
	}
}

void SettingsWindow::saveSettings()
{
	saveRayTracerSettings();

	saveCameraSettings();

	if (ui->lvLights->currentIndex().isValid())
	{
		saveSelectedLightSettings();
	}

	if (ui->lvMaterials->currentIndex().isValid())
	{
		saveSelectedMaterialSettings();
	}
	
	if (ui->tvShapes->currentIndex().isValid())
	{
		saveSelectedShapeSettings();
	}
}

bool SettingsWindow::checkCompositeExpressionValidity(const QString& expression, const QModelIndex& index) const
{
	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());

	std::vector<ComponentShape*> shapes = Application::m_scene.getShapes();
	auto it = std::find(shapes.begin(), shapes.end(), shape);
	if (it != shapes.end())
	{
		shapes.erase(it);
	}
	
	CompositeShape compositeShape;
    compositeShape.fromExpression(expression.toStdString(), shapes);

    if (compositeShape.hasChildren())
    {
		return true;
    }
    else
    {
		return false;
    }
}

bool SettingsWindow::checkNameValidity(const QString& name, const QAbstractItemView* view, const QModelIndex& index) const
{
	if (name.isEmpty() ||
		name.contains('&') ||
		name.contains('|') ||
		name.contains('-') ||
		name.contains('/') ||
		name.contains('(') ||
		name.contains(')') ||
		name.contains(' '))
	{
		return false;
	}

	QModelIndexList indexList = view->model()->match(view->model()->index(0, 0), Qt::DisplayRole, name, -1, Qt::MatchFixedString | Qt::MatchWrap | Qt::MatchRecursive);
	auto it = std::find_if(indexList.begin(), indexList.end(), [&index](const QModelIndex& matchedIndex)
	{
		return matchedIndex != index;
	});
	if (it != indexList.end())
	{
		return false;
	}
	
	return true;
}

QString SettingsWindow::createUniqueItemName(const QAbstractItemView* view, const QString& desiredName) const
{
	QString name = desiredName;

	int i = 1;
	while (!view->model()->match(view->model()->index(0, 0), Qt::DisplayRole, name, 1, Qt::MatchFixedString | Qt::MatchWrap | Qt::MatchRecursive).empty())
	{
		i++;
		if (int j = name.lastIndexOf("_"); j != -1)
		{
			name = name.left(j);
		}
		name = name + "_" + QString::number(i);
	}
	
	return name;
}

QVector<ComponentShape*> SettingsWindow::getExpandedShapes(const QTreeView* treeView, const QModelIndex& parentIndex, const QStandardItemModel& model) const
{
	QVector<ComponentShape*> expandedShapes;

	for (int i = 0; i < model.rowCount(parentIndex); ++i)
	{
		QModelIndex index = model.index(i, 0, parentIndex);
		if (treeView->isExpanded(index))
		{
			ComponentShape* shape = static_cast<ComponentShape*>(model.data(index, Qt::UserRole).value<void*>());
			expandedShapes.push_back(shape);
		}
		expandedShapes += getExpandedShapes(treeView, index, model);
	}

	return expandedShapes;
}

void SettingsWindow::expandShapes(QTreeView* treeView, const QModelIndex& parentIndex, const QStandardItemModel& model, const QVector<ComponentShape*>& shapes) 
{
	for (int i = 0; i < model.rowCount(parentIndex); ++i)
	{
		QModelIndex index = model.index(i, 0, parentIndex);
		ComponentShape* shape = static_cast<ComponentShape*>(model.data(index, Qt::UserRole).value<void*>());
		if (shapes.indexOf(shape) >= 0)
		{
			treeView->expand(index);
		}
		expandShapes(treeView, index, model, shapes);
	}
}

void SettingsWindow::selectShape(QTreeView* treeView, const QModelIndex& parentIndex, const QStandardItemModel& model, const ComponentShape* shape)
{
	for (int i = 0; i < model.rowCount(parentIndex); ++i)
	{
		QModelIndex index = model.index(i, 0, parentIndex);
		ComponentShape* sh = static_cast<ComponentShape*>(model.data(index, Qt::UserRole).value<void*>());
		if (sh == shape)
		{
			treeView->setCurrentIndex(index);
		}
		else
		{
			selectShape(treeView, index, model, shape);
		}
	}
}

void SettingsWindow::loadRayTracerSettings()
{
	QColor color = Utility::colorToQColor(Application::m_rayTracer.getBackgroundColor());
	ui->btnBackColor->setStyleSheet("border: 1px solid black; background-color: " + color.name());
	ui->chkSuper->setChecked(Application::m_rayTracer.getAdaptiveSuperSampling());
	ui->sbTreshold->setValue(Application::m_rayTracer.getSuperSamplingThreshold());
	ui->sbSubsampling->setValue(Application::m_rayTracer.getSubSamplingSize());
	ui->sbRecursion->setValue(Application::m_rayTracer.getRecursion());
	ui->sbShadowCount->setValue(Application::m_rayTracer.getShadowDist());
	ui->sbReflectionCount->setValue(Application::m_rayTracer.getReflectionDist());
	ui->sbRefractionCount->setValue(Application::m_rayTracer.getRefractionDist());
}

void SettingsWindow::loadCameraSettings()
{
	ui->sbCamRotSpeed->setValue(Application::m_scene.getCamera()->getRotationCoef());
	ui->sbCameraStepSize->setValue(Application::m_scene.getCamera()->getStepSize());
	PerspectiveCamera* perspectiveCamera = dynamic_cast<PerspectiveCamera*>(Application::m_scene.getCamera());
	OrthoCamera* orthoCamera = dynamic_cast<OrthoCamera*>(Application::m_scene.getCamera());
	ui->rbPerspective->blockSignals(true);
	ui->rbOrtho->blockSignals(true);
	ui->rbPerspective->setChecked(perspectiveCamera);
	ui->rbOrtho->setChecked(orthoCamera);
	ui->labelFOV->setVisible(perspectiveCamera);
	ui->sbFOV->setVisible(perspectiveCamera);
	ui->labelOrthoWidth->setVisible(orthoCamera);
	ui->sbOrthoWidth->setVisible(orthoCamera);
	ui->labelOrthoHeight->setVisible(orthoCamera);
	ui->sbOrthoHeight->setVisible(orthoCamera);
	if (perspectiveCamera != nullptr)
	{
		ui->sbFOV->setValue(perspectiveCamera->getFOV());

	}
	else if (orthoCamera != nullptr)
	{
		ui->sbOrthoWidth->setValue(orthoCamera->getOrthoWidth());
		ui->sbOrthoHeight->setValue(orthoCamera->getOrthoHeight());
	}
	ui->rbPerspective->blockSignals(false);
	ui->rbOrtho->blockSignals(false);
	const Vector& position = Application::m_scene.getCamera()->getPosition();
	ui->sbCamX->setValue(position.m_x);
	ui->sbCamY->setValue(position.m_y);
	ui->sbCamZ->setValue(position.m_z);
}

void SettingsWindow::on_cbLightType_currentIndexChanged(const QString& arg1)
{
	QModelIndex index = ui->lvLights->currentIndex();
	QStandardItem* item = m_lightListModel.itemFromIndex(index);

	Light* light = static_cast<Light*>(item->data(Qt::UserRole).value<void*>());

    if (ui->cbLightType->currentText() == "Sphere light" && !dynamic_cast<SphereLight*>(light))
    {
		SphereLight sphereLight;
		light = Application::m_scene.setLight(light, sphereLight);
		item->setData(QVariant::fromValue(static_cast<void*>(light)), Qt::UserRole);
		saveSelectedLightSettings();
		loadSelectedLightSettings();
    }
    else if (ui->cbLightType->currentText() == "Point light" && !dynamic_cast<PointLight*>(light))
    {
		PointLight pointLight;
		light = Application::m_scene.setLight(light, pointLight);
		item->setData(QVariant::fromValue(static_cast<void*>(light)), Qt::UserRole);
		saveSelectedLightSettings();
		loadSelectedLightSettings();
    }
}

void SettingsWindow::on_btnDeleteMat_clicked()
{
	QModelIndex index = ui->lvMaterials->currentIndex();
	Material* material = static_cast<Material*>(index.data(Qt::UserRole).value<void*>());

	Application::m_scene.eraseMaterial(material);
	loadMaterialItems();
}

void SettingsWindow::on_currentMaterialChanged(const QModelIndex& current, const QModelIndex& previous)
{
	QModelIndex index = ui->lvMaterials->currentIndex();

	if (!index.isValid())
	{
		ui->btnCopyMat->setEnabled(false);
		ui->btnDeleteMat->setEnabled(false);
		ui->itemDataTabWidget->setVisible(false);
	}
	else
	{
		ui->lvLights->setCurrentIndex(QModelIndex());;
		ui->tvShapes->setCurrentIndex(QModelIndex());;

		ui->btnCopyMat->setEnabled(true);
		ui->btnDeleteMat->setEnabled(true);

		loadSelectedMaterialSettings();
	}
}

void SettingsWindow::on_lvMaterialsDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	if (!roles.contains(Qt::DisplayRole)) return;

	QString newName = topLeft.data(Qt::DisplayRole).toString().trimmed();

	if (checkNameValidity(newName, ui->lvMaterials, topLeft))
	{
		Material* material = static_cast<Material*>(topLeft.data(Qt::UserRole).value<void*>());
		material->setName(newName.toStdString());
		loadSelectedMaterialSettings();
	}
	else
	{
		loadMaterialItems();
	}
}

void SettingsWindow::on_btnAddMat_clicked()
{
	ui->lvLights->setCurrentIndex(QModelIndex());;
	ui->tvShapes->setCurrentIndex(QModelIndex());;

	QString name = createUniqueItemName(ui->lvMaterials, DEFAULT_MATERIAL_NAME);
	Material* material = Application::m_scene.addMaterial(SimpleMaterial(MaterialProperties(Color(1, 0.5, 0.3), Color(0.5, 0.5, 0.5), Color(0.1, 0.05, 0.03), 32), name.toStdString()));
	
	addMaterialItem(material);

	QModelIndex newIndex = m_materialListModel.index(m_materialListModel.rowCount() - 1, 0);
	ui->lvMaterials->setCurrentIndex(newIndex);
}

void SettingsWindow::on_btnLightColor_clicked()
{
	QColor initColor = Utility::styleSheetToQColor(ui->btnLightColor->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

	if (color.isValid())
	{
		ui->btnLightColor->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedLightSettings();
	}
}

void SettingsWindow::addLightItem(Light* light)
{
	QStandardItem* item = new QStandardItem(QString::fromStdString(light->getName()));
	item->setData(QVariant::fromValue(static_cast<void*>(light)), Qt::UserRole);
	if (light->isEnabled())
	{
		item->setData(QBrush(Qt::black), Qt::ForegroundRole);
	}
	else
	{
		item->setData(QBrush(Qt::gray), Qt::ForegroundRole);
	}
	m_lightListModel.appendRow(item);
}

void SettingsWindow::addMaterialItem(Material* material)
{
	QStandardItem* item = new QStandardItem(QString::fromStdString(material->getName()));
	item->setData(QVariant::fromValue(static_cast<void*>(material)), Qt::UserRole);
	m_materialListModel.appendRow(item);

	QStandardItem* comboBoxItem = new QStandardItem(QString::fromStdString(material->getName()));
	comboBoxItem->setData(QVariant::fromValue(static_cast<void*>(material)), Qt::UserRole);
	m_materialComboBoxModel.appendRow(comboBoxItem);
}

void SettingsWindow::addShapeItem(ComponentShape* shape, QStandardItem* rootItem)
{
	ComponentShape::Operation operation = shape->getOperation();
	QString itemName = QString::fromStdString(shape->getName());
	if (itemName.isEmpty())
	{
		switch (operation)
		{
		case ComponentShape::Operation::ADD:
			itemName = "|";
			break;
		case ComponentShape::Operation::SUBTRACT:
			itemName = "-";
			break;
		case ComponentShape::Operation::INTERSECT:
			itemName = "&";
			break;
		case ComponentShape::Operation::CLIP:
			itemName = "/";
			break;
		default:
			break;
		}
	}
	QStandardItem* item = new QStandardItem(itemName);
	item->setData(QVariant::fromValue(static_cast<void*>(shape)), Qt::UserRole);
	if (shape->isEnabled())
	{
		item->setData(QBrush(Qt::black), Qt::ForegroundRole);
	}
	else
	{
		item->setData(QBrush(Qt::gray), Qt::ForegroundRole);
	}
	if (rootItem != nullptr)
	{
		rootItem->setChild(rootItem->rowCount(), item);
	}
	else
	{
		m_shapeTreeModel.appendRow(item);
	}
	if (ComponentShape* leftChild = shape->getLeftChild(); leftChild != nullptr)
	{
		addShapeItem(leftChild, item);
	}
	if (ComponentShape* rightChild = shape->getRightChild(); rightChild != nullptr)
	{
		addShapeItem(rightChild, item);
	}
}

void SettingsWindow::loadLightItems()
{
	QModelIndex selectedIndex = ui->lvLights->currentIndex();
	Light* selectedLight = nullptr;
	if (selectedIndex.isValid())
	{
		selectedLight = static_cast<Light*>(selectedIndex.data(Qt::UserRole).value<void*>());
	}

	m_lightListModel.clear();

	std::vector<Light*> sceneLights = Application::m_scene.getLights();

	for (Light* sceneLight : sceneLights)
	{
		addLightItem(sceneLight);

		if (sceneLight == selectedLight)
		{
			QModelIndex index = m_lightListModel.index(m_lightListModel.rowCount() - 1, 0);
			ui->lvLights->setCurrentIndex(index);
		}
	}
}

void SettingsWindow::loadMaterialItems()
{
	QModelIndex selectedLVIndex = ui->lvMaterials->currentIndex();
	Material* selectedLVMaterial = nullptr;
	if (selectedLVIndex.isValid())
	{
		selectedLVMaterial = static_cast<Material*>(selectedLVIndex.data(Qt::UserRole).value<void*>());
	}

	Material* selectedCBMaterial = static_cast<Material*>(ui->cbMat->currentData(Qt::UserRole).value<void*>());

	ui->cbMat->blockSignals(true);

	m_materialListModel.clear();
	m_materialComboBoxModel.clear();

	QStandardItem* item = new QStandardItem("None");
	item->setData(QVariant::fromValue(static_cast<void*>(nullptr)), Qt::UserRole);
	m_materialComboBoxModel.appendRow(item);

	std::vector<Material*> sceneMaterials = Application::m_scene.getMaterials();

	for (Material* sceneMaterial : sceneMaterials)
	{
		addMaterialItem(sceneMaterial);

		if (sceneMaterial == selectedLVMaterial)
		{
			QModelIndex index = m_materialListModel.index(m_materialListModel.rowCount() - 1, 0);
			ui->lvMaterials->setCurrentIndex(index);
		}

		if (sceneMaterial == selectedCBMaterial)
		{
			ui->cbMat->setCurrentIndex(m_materialComboBoxModel.rowCount() - 1);
		}
	}

	ui->cbMat->blockSignals(false);
}

void SettingsWindow::loadShapeItems()
{
	QModelIndex selectedIndex = ui->tvShapes->currentIndex();
	ComponentShape* selectedShape = nullptr;
	if (selectedIndex.isValid())
	{
		selectedShape = static_cast<ComponentShape*>(selectedIndex.data(Qt::UserRole).value<void*>());
	}

	QVector<ComponentShape*> expandedShapes = getExpandedShapes(ui->tvShapes, QModelIndex(), m_shapeTreeModel);
	
	m_shapeTreeModel.clear();

	std::vector<ComponentShape*> sceneShapes = Application::m_scene.getShapes();

	for (ComponentShape* sceneShape : sceneShapes)
	{
		addShapeItem(sceneShape, nullptr);
	}

	expandShapes(ui->tvShapes, QModelIndex(), m_shapeTreeModel, expandedShapes);

	selectShape(ui->tvShapes, QModelIndex(), m_shapeTreeModel, selectedShape);
}

void SettingsWindow::on_cbMatType_currentIndexChanged(const QString& arg1)
{  
	QModelIndex index = ui->lvMaterials->currentIndex();
	QStandardItem* matListItem = m_materialListModel.itemFromIndex(index);
	QStandardItem* matComboBoxItem = m_materialComboBoxModel.item(index.row() + 1);

	Material* material = static_cast<Material*>(matListItem->data(Qt::UserRole).value<void*>());

	if (ui->cbMatType->currentText() == "Simple" && !dynamic_cast<SimpleMaterial*>(material))
	{
		SimpleMaterial simpleMaterial;

		simpleMaterial.setName(ui->lnMatName->text().toStdString());

		material = Application::m_scene.setMaterial(material, simpleMaterial);

		matListItem->setData(QVariant::fromValue(static_cast<void*>(material)), Qt::UserRole);
		matComboBoxItem->setData(QVariant::fromValue(static_cast<void*>(material)), Qt::UserRole);

		loadSelectedMaterialSettings();
	}
	else if (ui->cbMatType->currentText() == "Checkerboard" && !dynamic_cast<CheckerMaterial*>(material))
	{
		CheckerMaterial checkerMaterial;

		checkerMaterial.setName(ui->lnMatName->text().toStdString());

		material = Application::m_scene.setMaterial(material, checkerMaterial);

		matListItem->setData(QVariant::fromValue(static_cast<void*>(material)), Qt::UserRole);
		matComboBoxItem->setData(QVariant::fromValue(static_cast<void*>(material)), Qt::UserRole);

		loadSelectedMaterialSettings();
	}
}

void SettingsWindow::on_btnBackColor_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnBackColor->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnBackColor->setStyleSheet("border: 1px solid black; background-color: " + color.name());
        saveRayTracerSettings();
    }
}

void SettingsWindow::on_btnMatColorDiffuse_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorDiffuse->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorDiffuse->setStyleSheet("border: 1px solid black; background-color: " + color.name());

        saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorSpecular_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorSpecular->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorSpecular->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorAmbient_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorAmbient->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorAmbient->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorDiffuse_2_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorDiffuse_2->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorDiffuse_2->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorSpecular_2_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorSpecular_2->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorSpecular_2->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorAmbient_2_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorAmbient_2->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorAmbient_2->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorDiffuse_3_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorDiffuse_3->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorDiffuse_3->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorSpecular_3_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorSpecular_3->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorSpecular_3->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_btnMatColorAmbient_3_clicked()
{
    QColor initColor = Utility::styleSheetToQColor(ui->btnMatColorAmbient_3->styleSheet());
	QColor color = QColorDialog::getColor(initColor, this, QString());

    if (color.isValid())
    {
        ui->btnMatColorAmbient_3->setStyleSheet("border: 1px solid black; background-color: " + color.name());

		saveSelectedMaterialSettings();
    }
}

void SettingsWindow::on_sbCamX_editingFinished()
{
    saveCameraSettings();
}

void SettingsWindow::on_sbCamY_editingFinished()
{
	saveCameraSettings();
}

void SettingsWindow::on_sbCamZ_editingFinished()
{
	saveCameraSettings();
}

void SettingsWindow::on_sbFOV_editingFinished()
{
	saveCameraSettings();
}

void SettingsWindow::on_sbCamRotSpeed_editingFinished()
{
	saveCameraSettings();
}

void SettingsWindow::on_sbCameraStepSize_editingFinished()
{
	saveCameraSettings();
}

void SettingsWindow::on_chkSuper_clicked()
{
    saveRayTracerSettings();
}

void SettingsWindow::on_sbTreshold_editingFinished()
{
	saveRayTracerSettings();
}

void SettingsWindow::on_sbSubsampling_editingFinished()
{
	saveRayTracerSettings();
}

void SettingsWindow::on_sbRecursion_editingFinished()
{
	saveRayTracerSettings();
}

void SettingsWindow::on_sbShadowCount_editingFinished()
{
	saveRayTracerSettings();
}

void SettingsWindow::on_sbReflectionCount_editingFinished()
{
	saveRayTracerSettings();
}

void SettingsWindow::on_sbRefractionCount_editingFinished()
{
	saveRayTracerSettings();
}

void SettingsWindow::on_sbLightXPos_editingFinished()
{
    saveSelectedLightSettings();
}

void SettingsWindow::on_sbLightYPos_editingFinished()
{
	saveSelectedLightSettings();
}

void SettingsWindow::on_sbLightZPos_editingFinished()
{
	saveSelectedLightSettings();
}

void SettingsWindow::on_sbLightRadius_editingFinished()
{
	saveSelectedLightSettings();
}

void SettingsWindow::on_sbMatShininess_editingFinished()
{
    saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatReflectance_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatReflectDistAngle_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatTransparency_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatRefIndex_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatRefractDistAngle_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatShininess_2_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatReflectance_2_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatReflectDistAngle_2_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatRefractDistAngle_2_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatTransparency_2_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatRefIndex_2_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatShininess_3_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatReflectance_3_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatReflectDistAngle_3_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatTransparency_3_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatRefIndex_3_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbMatRefractDistAngle_3_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbCheckerTileX_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbCheckerTileY_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_sbCheckerTileZ_editingFinished()
{
	saveSelectedMaterialSettings();
}

void SettingsWindow::on_btnDeleteShape_clicked()
{
	QModelIndex index = ui->tvShapes->currentIndex();
	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());

	Application::m_scene.eraseShape(shape);

	loadShapeItems();
}

void SettingsWindow::on_currentShapeChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (QModelIndex index = ui->tvShapes->currentIndex(); !index.isValid())
	{
		ui->btnCopyShape->setEnabled(false);
		ui->btnDeleteShape->setEnabled(false);
		ui->itemDataTabWidget->setVisible(false);
	}
	else
	{
		ui->lvLights->setCurrentIndex(QModelIndex());;
		ui->lvMaterials->setCurrentIndex(QModelIndex());;

		ui->sbTransX->setValue(0.0);
		ui->sbTransY->setValue(0.0);
		ui->sbTransZ->setValue(0.0);
		ui->sbRotX->setValue(0.0);
		ui->sbRotY->setValue(0.0);
		ui->sbRotZ->setValue(0.0);
		ui->sbScaleX->setValue(1.0);
		ui->sbScaleY->setValue(1.0);
		ui->sbScaleZ->setValue(1.0);

		ui->btnCopyShape->setEnabled(true);
		ui->btnDeleteShape->setEnabled(true);

		loadSelectedShapeSettings();
	}
}

void SettingsWindow::on_tvShapesDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
	if (!roles.contains(Qt::DisplayRole)) return;

	QString newName = topLeft.data(Qt::DisplayRole).toString().trimmed();

	if (checkNameValidity(newName, ui->tvShapes, topLeft))
	{
		ComponentShape* shape = static_cast<ComponentShape*>(topLeft.data(Qt::UserRole).value<void*>());
		shape->setName(newName.toStdString());
		loadSelectedShapeSettings();
	}
	else
	{
		loadShapeItems();
	}
}

void SettingsWindow::on_btnAddShape_clicked()
{
	ui->lvLights->setCurrentIndex(QModelIndex());;
	ui->lvMaterials->setCurrentIndex(QModelIndex());;

	QString name = createUniqueItemName(ui->tvShapes, DEFAULT_SHAPE_NAME);
	Quadric quadric = Quadric(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, nullptr, nullptr, name.toStdString());
	std::vector<Material*> materials = Application::m_scene.getMaterials();
	if (!materials.empty())
	{
		quadric.setMaterial(materials[0]);
	}
	ComponentShape* shape = Application::m_scene.addShape(quadric);
	
	addShapeItem(shape, nullptr);

	QModelIndex newIndex = m_shapeTreeModel.index(m_shapeTreeModel.rowCount() - 1, 0);
	ui->tvShapes->setCurrentIndex(newIndex);
}

void SettingsWindow::on_btnSetShape_clicked()
{
    if (ui->cbShape->currentText() == "Sphere")
    {
        ui->sbA->setValue(1.0);
        ui->sbB->setValue(1.0);
        ui->sbC->setValue(1.0);
        ui->sbD->setValue(0.0);
        ui->sbE->setValue(0.0);
        ui->sbF->setValue(0.0);
        ui->sbG->setValue(0.0);
        ui->sbH->setValue(0.0);
        ui->sbI->setValue(0.0);
        ui->sbJ->setValue(-1.0);
    }
    else if (ui->cbShape->currentText() == "Cylinder")
    {
        ui->sbA->setValue(1.0);
        ui->sbB->setValue(0.0);
        ui->sbC->setValue(1.0);
        ui->sbD->setValue(0.0);
        ui->sbE->setValue(0.0);
        ui->sbF->setValue(0.0);
        ui->sbG->setValue(0.0);
        ui->sbH->setValue(0.0);
        ui->sbI->setValue(0.0);
        ui->sbJ->setValue(-1.0);
    }
    else if (ui->cbShape->currentText() == "Cone")
    {
        ui->sbA->setValue(1.0);
        ui->sbB->setValue(-1.0);
        ui->sbC->setValue(1.0);
        ui->sbD->setValue(0.0);
        ui->sbE->setValue(0.0);
        ui->sbF->setValue(0.0);
        ui->sbG->setValue(0.0);
        ui->sbH->setValue(0.0);
        ui->sbI->setValue(0.0);
        ui->sbJ->setValue(0.0);
    }
    else if (ui->cbShape->currentText() == "Plane")
    {
        ui->sbA->setValue(0.0);
        ui->sbB->setValue(0.0);
        ui->sbC->setValue(0.0);
        ui->sbD->setValue(0.0);
        ui->sbE->setValue(0.0);
        ui->sbF->setValue(0.0);
        ui->sbG->setValue(0.0);
        ui->sbH->setValue(1.0);
        ui->sbI->setValue(0.0);
        ui->sbJ->setValue(0.0);
    }
    else if (ui->cbShape->currentText() == "Paraboloid")
    {
        ui->sbA->setValue(1.0);
        ui->sbB->setValue(0.0);
        ui->sbC->setValue(1.0);
        ui->sbD->setValue(0.0);
        ui->sbE->setValue(0.0);
        ui->sbF->setValue(0.0);
        ui->sbG->setValue(0.0);
        ui->sbH->setValue(-1.0);
        ui->sbI->setValue(0.0);
        ui->sbJ->setValue(0.0);
    }
    else if (ui->cbShape->currentText() == "Hyperboloid")
    {
        ui->sbA->setValue(1.0);
        ui->sbB->setValue(-1.0);
        ui->sbC->setValue(1.0);
        ui->sbD->setValue(0.0);
        ui->sbE->setValue(0.0);
        ui->sbF->setValue(0.0);
        ui->sbG->setValue(0.0);
        ui->sbH->setValue(0.0);
        ui->sbI->setValue(0.0);
        ui->sbJ->setValue(-1.0);
    }

    saveSelectedShapeSettings();
}

void SettingsWindow::on_cbMat_currentIndexChanged(const QString& arg1)
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbA_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbB_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbC_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbD_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbE_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbF_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbG_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbH_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbI_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbJ_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbPlaneOriginX_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbPlaneOriginY_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbPlaneOriginZ_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbPlaneNormalX_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbPlaneNormalY_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbPlaneNormalZ_editingFinished()
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_cbShapeType_currentIndexChanged(const QString& arg1)
{
	QModelIndex index = ui->tvShapes->currentIndex();
	QStandardItem* item = m_shapeTreeModel.itemFromIndex(index);

	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());

	if (ui->cbShapeType->currentText() == "Quadric" && !dynamic_cast<Quadric*>(shape))
	{
		Quadric quadric = Quadric(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, nullptr, nullptr, shape->getName());
		quadric.setEnabled(ui->chkShapeEnabled->isChecked());
		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		quadric.setMaterial(material);

		shape = Application::m_scene.setShape(shape, quadric);

		item->setData(QVariant::fromValue(static_cast<void*>(shape)), Qt::UserRole);

		ui->cbShape->setCurrentText("Sphere");

		loadSelectedShapeSettings();
	}
	else if (ui->cbShapeType->currentText() == "Plane" && !dynamic_cast<Plane*>(shape))
	{
		Plane plane = Plane(Vector(0.0, 1.0, 0.0), Vector(0.0, 0.0, 0.0), nullptr, nullptr, shape->getName());
		plane.setEnabled(ui->chkShapeEnabled->isChecked());
		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		plane.setMaterial(material);

		shape = Application::m_scene.setShape(shape, plane);

		item->setData(QVariant::fromValue(static_cast<void*>(shape)), Qt::UserRole);

		loadSelectedShapeSettings();
	}
	else if (ui->cbShapeType->currentText() == "Model" && !dynamic_cast<Model*>(shape))
	{
		Model model;
		model.setName(shape->getName());
		model.setEnabled(ui->chkShapeEnabled->isChecked());
		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		model.setMaterial(material);

		shape = Application::m_scene.setShape(shape, model);

		item->setData(QVariant::fromValue(static_cast<void*>(shape)), Qt::UserRole);

		loadSelectedShapeSettings();
	}
	else if (ui->cbShapeType->currentText() == "CSG object" && !dynamic_cast<CompositeShape*>(shape))
	{
		CompositeShape compositeShape;
		compositeShape.setName(shape->getName());
		compositeShape.setEnabled(ui->chkShapeEnabled->isChecked());
		int i = ui->cbMat->currentIndex();
		QModelIndex materialIndex = m_materialComboBoxModel.index(i, 0);
		Material* material = static_cast<Material*>(materialIndex.data(Qt::UserRole).value<void*>());
		compositeShape.setMaterial(material);

		shape = Application::m_scene.setShape(shape, compositeShape);

		item->setData(QVariant::fromValue(static_cast<void*>(shape)), Qt::UserRole);

		loadSelectedShapeSettings();
	}
}

void SettingsWindow::on_btnCreateComposite_clicked()
{
	QModelIndex index = ui->tvShapes->currentIndex();
	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());

    QString text = ui->lnCompositeExpression->text().trimmed();
    ui->lnCompositeExpression->setText(text);
    ui->lnCompositeExpression->setStyleSheet("color : #000000");

	std::vector<ComponentShape*> sceneShapes = Application::m_scene.getShapes();
	auto it = std::find(sceneShapes.begin(), sceneShapes.end(), shape);
	if (it != sceneShapes.end())
	{
		sceneShapes.erase(it);
	}

	shape->fromExpression(ui->lnCompositeExpression->text().toStdString(), sceneShapes);

	std::vector<ComponentShape*> children = shape->getAllChildren();
	sceneShapes = Application::m_scene.getShapes();
	for (ComponentShape* sceneShape : sceneShapes)
	{
		for (const ComponentShape* child : children)
		{
			if (child->getName() == sceneShape->getName())
			{
				Application::m_scene.eraseShape(sceneShape);
				break;
			}
		}
	}
	
    loadSelectedShapeSettings();
}

void SettingsWindow::on_btnBreakComposite_clicked()
{
	QModelIndex index = ui->tvShapes->currentIndex();

	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());

	ComponentShape* leftChild = shape->getLeftChild();
	if (leftChild != nullptr)
	{
		std::unique_ptr<ComponentShape> leftChildCopy = leftChild->clone();
		ComponentShape* ptr = Application::m_scene.addShape(*leftChildCopy);
	}

	ComponentShape* rightChild = shape->getRightChild();
	if (rightChild != nullptr)
	{
		std::unique_ptr<ComponentShape> rightChildCopy = rightChild->clone();
		ComponentShape* ptr = Application::m_scene.addShape(*rightChildCopy);
	}

	Application::m_scene.eraseShape(shape);
	
	loadShapeItems();
}

void SettingsWindow::on_btnCopyLight_clicked()
{
	QModelIndex index = ui->lvLights->currentIndex();
	
	Light* oldLight = static_cast<Light*>(index.data(Qt::UserRole).value<void*>());
	QString oldName = QString::fromStdString(oldLight->getName());
	std::unique_ptr<Light> newLight = oldLight->clone();
	newLight->setName(createUniqueItemName(ui->lvLights, oldName).toStdString());
	Light* pNewLight = Application::m_scene.addLight(*newLight);
		
	loadLightItems();

	ui->lvLights->setCurrentIndex(m_lightListModel.index(m_lightListModel.rowCount() - 1, 0));
}

void SettingsWindow::on_btnCopyMat_clicked()
{
	QModelIndex index = ui->lvMaterials->currentIndex();

	Material* oldMaterial = static_cast<Material*>(index.data(Qt::UserRole).value<void*>());
	QString oldName = QString::fromStdString(oldMaterial->getName());
	std::unique_ptr<Material> newMaterial = oldMaterial->clone();
	newMaterial->setName(createUniqueItemName(ui->lvMaterials, oldName).toStdString());
	Material* pNewMaterial = Application::m_scene.addMaterial(*newMaterial);

	loadMaterialItems();

	ui->lvMaterials->setCurrentIndex(m_materialListModel.index(m_materialListModel.rowCount() - 1, 0));
}

void SettingsWindow::on_btnCopyShape_clicked()
{
	QModelIndex index = ui->tvShapes->currentIndex();

	ComponentShape* oldShape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());
	QString oldName = QString::fromStdString(oldShape->getName());
	std::unique_ptr<ComponentShape> newShape = oldShape->clone();
	newShape->setName(createUniqueItemName(ui->tvShapes, oldName).toStdString());

	if (ui->cbShapeType->currentText() == "Composite object")
	{
		std::vector<ComponentShape*> subShapes = newShape->getAllChildren();
		for (ComponentShape* subShape : subShapes)
		{
			subShape->setName(createUniqueItemName(ui->tvShapes, QString::fromStdString(subShape->getName())).toStdString());
		}
	}

	ComponentShape* pNewShape = Application::m_scene.addShape(*newShape);
	
	loadShapeItems();

	ui->tvShapes->setCurrentIndex(m_shapeTreeModel.index(m_shapeTreeModel.rowCount() - 1, 0));
}

void SettingsWindow::on_lnCompositeExpression_textEdited(const QString& arg1)
{
	if (checkCompositeExpressionValidity(arg1.trimmed(), ui->tvShapes->currentIndex()) == true)
	{
		ui->lnCompositeExpression->setStyleSheet("color : #000000");
		ui->btnCreateComposite->setEnabled(true);
	}
	else
	{
		ui->lnCompositeExpression->setStyleSheet("color : #FF0000");
		ui->btnCreateComposite->setEnabled(false);
	}
}

void SettingsWindow::on_btnShapeNameSet_clicked()
{
	saveSelectedShapeSettings();

	loadShapeItems();
}

void SettingsWindow::on_lnShapeName_textEdited(const QString& arg1)
{
	QModelIndex index = ui->tvShapes->currentIndex();
	QString newName = ui->lnShapeName->text().trimmed();

	if (checkNameValidity(newName, ui->tvShapes, index))
	{
		ui->lnShapeName->setStyleSheet("color : #000000");
		ui->btnShapeNameSet->setEnabled(true);
	}
	else
	{
		ui->lnShapeName->setStyleSheet("color : #FF0000");
		ui->btnShapeNameSet->setEnabled(false);
	}
}

void SettingsWindow::on_btnLightNameSet_clicked()
{
	saveSelectedLightSettings();

	loadLightItems();
}

void SettingsWindow::on_btnMatNameSet_clicked()
{
	saveSelectedMaterialSettings();

	loadMaterialItems();
}

void SettingsWindow::on_lnLightName_textEdited(const QString& arg1)
{
	QModelIndex index = ui->lvLights->currentIndex();
	QString newName = ui->lnLightName->text().trimmed();

	if (checkNameValidity(newName, ui->lvLights, index))
	{
		ui->lnLightName->setStyleSheet("color : #000000");
		ui->btnLightNameSet->setEnabled(true);
	}
	else
	{
		ui->lnLightName->setStyleSheet("color : #FF0000");
		ui->btnLightNameSet->setEnabled(false);
	}
}

void SettingsWindow::on_lnMatName_textEdited(const QString& arg1)
{
	QModelIndex index = ui->lvMaterials->currentIndex();
	QString newName = ui->lnMatName->text().trimmed();

	if (checkNameValidity(newName, ui->lvMaterials, index))
	{
		ui->lnMatName->setStyleSheet("color : #000000");
		ui->btnMatNameSet->setEnabled(true);
	}
	else
	{
		ui->lnMatName->setStyleSheet("color : #FF0000");
		ui->btnMatNameSet->setEnabled(false);
	}
}
void SettingsWindow::on_chkLightEnabled_clicked(bool checked)
{
	saveSelectedLightSettings();

	loadLightItems();
}

void SettingsWindow::on_chkShapeEnabled_clicked(bool checked)
{
	saveSelectedShapeSettings();

	loadShapeItems();
}

void SettingsWindow::on_btnBrowseModel_clicked()
{
	std::vector<std::string> extensions = AssimpModelLoader::getImportExtensions();
	QString filters = "All files (*.*);;";
	for (const std::string& extension : extensions)
	{
		filters += "(" + QString::fromStdString(extension) + ");;";
	}
	QString selectedFilter = "All files (*.*)";

	QString fileName = QFileDialog::getOpenFileName(this, "Load Model", "", filters, &selectedFilter);
    if (!fileName.isEmpty())
    {
		Model newModel = AssimpModelLoader::loadModel(fileName.toStdString());
        if (newModel.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setText("Unable to load model from file " + fileName);
            msgBox.exec();
        }
        else
        {
			QModelIndex index = ui->tvShapes->currentIndex();
			Model* oldModel = static_cast<Model*>(index.data(Qt::UserRole).value<void*>());
			*oldModel = newModel;

			saveSelectedShapeSettings();

			loadSelectedShapeSettings();
        }      
    }
}

void SettingsWindow::on_chkSmooth_clicked(bool checked)
{
	saveSelectedShapeSettings();
}

void SettingsWindow::on_sbOrthoWidth_editingFinished()
{
    saveCameraSettings();
}

void SettingsWindow::on_sbOrthoHeight_editingFinished()
{
    saveCameraSettings();
}

void SettingsWindow::on_rbPerspective_toggled(bool checked)
{
    saveCameraSettings();

	loadCameraSettings();
}

void SettingsWindow::on_rbOrtho_toggled(bool checked)
{
	saveCameraSettings();

	loadCameraSettings();
}

void SettingsWindow::on_btnTransApply_clicked()
{
	QModelIndex index = ui->tvShapes->currentIndex();
	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());
	shape->translate(Vector(ui->sbTransX->value(), ui->sbTransY->value(), ui->sbTransZ->value()));
	
    loadSelectedShapeSettings();
}

void SettingsWindow::on_btnRotApply_clicked()
{
	QModelIndex index = ui->tvShapes->currentIndex();
	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());
	shape->rotate(ui->sbRotAmount->value(), Vector(ui->sbRotX->value(), ui->sbRotY->value(), ui->sbRotZ->value()));

	loadSelectedShapeSettings();
}

void SettingsWindow::on_btnScaleApply_clicked()
{
	QModelIndex index = ui->tvShapes->currentIndex();
	ComponentShape* shape = static_cast<ComponentShape*>(index.data(Qt::UserRole).value<void*>());
	shape->scale(Vector(ui->sbScaleX->value(), ui->sbScaleY->value(), ui->sbScaleZ->value()));

	loadSelectedShapeSettings();
}
