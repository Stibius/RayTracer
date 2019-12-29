#pragma once

#include <QMainWindow>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QString>
#include <QVector>
#include <QTreeView>

#include "ComponentShape.h"
#include "Light.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:

	explicit SettingsWindow(QWidget* parent = nullptr);

	SettingsWindow(const SettingsWindow&) = delete;

	SettingsWindow(SettingsWindow&&) = delete;

	SettingsWindow& operator=(const SettingsWindow&) = delete;

	SettingsWindow& operator=(SettingsWindow&&) = delete;

    ~SettingsWindow() override;

	void loadSettings();

public slots:

    void on_cameraPosChanged();

private slots:

	void on_currentLightChanged(const QModelIndex& current, const QModelIndex& previous);

	void on_lvLightsDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

	void on_btnAddLight_clicked();

	void on_btnCopyLight_clicked();

	void on_btnDeleteLight_clicked();


	void on_currentMaterialChanged(const QModelIndex& current, const QModelIndex& previous);

	void on_lvMaterialsDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

	void on_btnAddMat_clicked();

	void on_btnCopyMat_clicked();

	void on_btnDeleteMat_clicked();


	void on_currentShapeChanged(const QModelIndex& current, const QModelIndex& previous);

	void on_tvShapesDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);

	void on_btnAddShape_clicked();

	void on_btnCopyShape_clicked();

	void on_btnDeleteShape_clicked();

   
    void on_cbLightType_currentIndexChanged(const QString& arg1);

    void on_btnLightColor_clicked();

	void on_sbLightXPos_editingFinished();

	void on_sbLightYPos_editingFinished();

	void on_sbLightZPos_editingFinished();

	void on_sbLightRadius_editingFinished();

	void on_lnLightName_textEdited(const QString& arg1);

	void on_btnLightNameSet_clicked();

	void on_chkLightEnabled_clicked(bool checked);

    
	void on_cbMatType_currentIndexChanged(const QString& arg1);

	void on_btnMatColorDiffuse_clicked();

	void on_btnMatColorSpecular_clicked();

	void on_btnMatColorAmbient_clicked();

	void on_btnMatColorDiffuse_2_clicked();

	void on_btnMatColorSpecular_2_clicked();

	void on_btnMatColorAmbient_2_clicked();

	void on_btnMatColorDiffuse_3_clicked();

	void on_btnMatColorSpecular_3_clicked();

	void on_btnMatColorAmbient_3_clicked();

	void on_sbMatShininess_editingFinished();

	void on_sbMatReflectance_editingFinished();

	void on_sbMatReflectDistAngle_editingFinished();

	void on_sbMatTransparency_editingFinished();

	void on_sbMatRefIndex_editingFinished();

	void on_sbMatRefractDistAngle_editingFinished();

	void on_sbMatShininess_2_editingFinished();

	void on_sbMatReflectance_2_editingFinished();

	void on_sbMatReflectDistAngle_2_editingFinished();

	void on_sbMatRefractDistAngle_2_editingFinished();

	void on_sbMatTransparency_2_editingFinished();

	void on_sbMatRefIndex_2_editingFinished();

	void on_sbMatShininess_3_editingFinished();

	void on_sbMatReflectance_3_editingFinished();

	void on_sbMatReflectDistAngle_3_editingFinished();

	void on_sbMatTransparency_3_editingFinished();

	void on_sbMatRefIndex_3_editingFinished();

	void on_sbMatRefractDistAngle_3_editingFinished();

	void on_sbCheckerTileX_editingFinished();

	void on_sbCheckerTileY_editingFinished();

	void on_sbCheckerTileZ_editingFinished();

	void on_lnMatName_textEdited(const QString& arg1);

	void on_btnMatNameSet_clicked();


	void on_btnSetShape_clicked();

	void on_cbMat_currentIndexChanged(const QString& arg1);

	void on_sbA_editingFinished();

	void on_sbB_editingFinished();

	void on_sbC_editingFinished();

	void on_sbD_editingFinished();

	void on_sbE_editingFinished();

	void on_sbF_editingFinished();

	void on_sbG_editingFinished();

	void on_sbH_editingFinished();

	void on_sbI_editingFinished();

	void on_sbJ_editingFinished();

	void on_sbPlaneOriginX_editingFinished();

	void on_sbPlaneOriginY_editingFinished();

	void on_sbPlaneOriginZ_editingFinished();

	void on_sbPlaneNormalX_editingFinished();

	void on_sbPlaneNormalY_editingFinished();

	void on_sbPlaneNormalZ_editingFinished();

	void on_cbShapeType_currentIndexChanged(const QString& arg1);

	void on_btnCreateComposite_clicked();

	void on_btnBreakComposite_clicked();

	void on_lnCompositeExpression_textEdited(const QString& arg1);

	void on_btnBrowseModel_clicked();

	void on_chkSmooth_clicked(bool checked);

	void on_btnShapeNameSet_clicked();

	void on_lnShapeName_textEdited(const QString& arg1);

	void on_chkShapeEnabled_clicked(bool checked);

	void on_btnTransApply_clicked();

	void on_btnRotApply_clicked();

	void on_btnScaleApply_clicked();

    
	void on_btnBackColor_clicked();


    void on_sbCamX_editingFinished();

    void on_sbCamY_editingFinished();

    void on_sbCamZ_editingFinished();

	void on_sbCamRotSpeed_editingFinished();

	void on_sbCameraStepSize_editingFinished();

    void on_sbFOV_editingFinished();

	void on_sbOrthoWidth_editingFinished();

	void on_sbOrthoHeight_editingFinished();

	void on_rbPerspective_toggled(bool checked);

	void on_rbOrtho_toggled(bool checked);


    void on_chkSuper_clicked();

    void on_sbTreshold_editingFinished();

    void on_sbSubsampling_editingFinished();

    void on_sbRecursion_editingFinished();

    void on_sbShadowCount_editingFinished();

    void on_sbReflectionCount_editingFinished();

    void on_sbRefractionCount_editingFinished();

private:

    Ui::SettingsWindow* ui;

	static const QString DEFAULT_LIGHT_NAME;
	static const QString DEFAULT_MATERIAL_NAME;
	static const QString DEFAULT_SHAPE_NAME;

	QStandardItemModel m_lightListModel;
	QStandardItemModel m_materialListModel;
	QStandardItemModel m_materialComboBoxModel;
	QStandardItemModel m_shapeTreeModel;

	void addLightItem(Light* light);

	void addMaterialItem(Material* material);

	void addShapeItem(ComponentShape* shape, QStandardItem* rootItem);

	bool checkCompositeExpressionValidity(const QString& expression, const QModelIndex& index) const;

	bool checkNameValidity(const QString& name, const QAbstractItemView* view, const QModelIndex& index) const;

	QString createUniqueItemName(const QAbstractItemView* view, const QString& desiredName) const;

	QVector<ComponentShape*> getExpandedShapes(const QTreeView* treeView, const QModelIndex& parentIndex, const QStandardItemModel& model) const;

	void expandShapes(QTreeView* treeView, const QModelIndex& parentIndex, const QStandardItemModel& model, const QVector<ComponentShape*>& shapes);

	void selectShape(QTreeView* treeView, const QModelIndex& parentIndex, const QStandardItemModel& model, const ComponentShape* shape);

	void loadRayTracerSettings();

	void loadCameraSettings();

	void loadLightItems();

	void loadMaterialItems();

	void loadShapeItems();

    void loadSelectedLightSettings();

    void loadSelectedMaterialSettings();

    void loadSelectedShapeSettings();

	void saveRayTracerSettings();

	void saveCameraSettings();

	void saveSelectedLightSettings();

	void saveSelectedMaterialSettings();

	void saveSelectedShapeSettings();

    void saveSettings();

};
