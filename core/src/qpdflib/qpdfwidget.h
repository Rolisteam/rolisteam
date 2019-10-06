/*
                          qpdf

    Copyright (C) 2015 Arthur Benilov,
    arthur.benilov@gmail.com
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.
*/

#ifndef QPDFWIDGET_H
#define QPDFWIDGET_H

#include <QWidget>

class QWebEngineView;

struct QPdfWidgetPrivate;

/**
 * @brief PDF visualization widget.
 *
 * This class represents a widget used to visualize a pdf file.
 * It handles pdf file loading but also accepts raw data (byte array)
 * to be interpreted as a pdf.
 */
class QPdfWidget : public QWidget
{
    Q_OBJECT
public:
    QPdfWidget(QWidget* pParent= nullptr);
    ~QPdfWidget();

    /**
     * @brief Load and display a pdf file.
     * @param path Absolute path to a pdf file.
     * @return true if loaded Ok, false if unable to load.
     */
    bool loadFile(const QString& path);

    /**
     * @brief Load raw pdf data.
     * @param data PDF data.
     */
    bool loadData(const QByteArray& data);

    /**
     * @brief Close PDF document.
     */
    void close();

    /**
     * @brief Set current page.
     *
     * This will make the viewer to navigate to a specific
     * page of the document.
     *
     * This method does not change the view when trying to navigate
     * outside of the document.
     *
     * @param page Page number.
     */
    void setPage(int page);

    /**
     * @brief Returns current page viewed.
     * @return Current page.
     */
    int page() const;

    /**
     * @brief Returns total number of pages in the document.
     * @return Total number of pages.
     */
    int pagesCount() const;

    /**
     * @brief Rotate all pages.
     *
     * Rotate current view by given angle.
     *
     * @param degrees Rotation delta, divisible by 90.
     */
    void rotatePages(int degrees);

    /**
     * @brief Zoom the view in.
     *
     * Single tick represent a zoom delta factor, which
     * is set internally by the PDF rendering library.
     *
     * @param ticks Number of zooming ticks.
     */
    void zoomIn(int ticks= 1);

    /**
     * @brief Zoom the view out.
     *
     * Single tick represent a zoom delta factor, which
     * is set internally by the PDF rendering library.
     *
     * @param ticks Number of zooming ticks.
     */
    void zoomOut(int ticks= 1);

    /**
     * @brief Reset zoom factor.
     * @param scale Zoom scale to be set.
     */
    void zoomReset(qreal scale= 1.0);

    /**
     * @brief Show PDF document properties pop-up window.
     */
    void showDocumentProperties();

    /**
     * @brief Set toolbar visibility.
     *
     * Toolbar is visible by default on the top of the PDF view.
     *
     * @param v true to show the toolbar, false to hide it.
     */
    void setToolbarVisible(bool v);

    /**
     * @brief Set find bar visibility.
     * @param v true to show the find bar, false to hide it.
     */
    void setFindBarVisible(bool v);

    /**
     * @brief Find text in the document.
     *
     * This method searches the text from the current view position
     * till the end of the document. Found text will be highlighted.
     *
     * The following occurrence of the text can be found via \ref findNext
     * or \ref findPrevious methods.
     *
     * @see findNext
     * @see findPrevious
     */
    void findText(const QString& text= QString());

    /**
     * @brief Find a next occurrence of the search string.
     *
     * This method continues the search initiated by \ref findText.
     *
     * @see findText
     */
    void findNext();

    /**
     * @brief Find previous occurrence of the search string.
     *
     * This method searches the text backwards for another occurrence
     * of the search string.
     *
     * @see findText
     */
    void findPrevious();

    /**
     * @brief Returns the number of matches found.
     *
     * @note This function will return zero when called
     * immediately after \ref findNext or \ref findPrevious.
     * This is due to the ascynchronous nature of the find controller.
     * One may introduce a small delay to make sure this function returns
     * a valid result.
     *
     * @return Number of found results.
     */
    int findResultsCount() const;

    /**
     * @brief Navigate to destination object.
     * @param dest PDF destination object.
     */
    void navigateTo(const QString& dest);

    /**
     * @brief Returns a list of available destinations.
     *
     * Returned strings are anchors to be used with \ref navigateTo method.
     *
     * @return List if navigation links (destinations).
     */
    QStringList getDestinations() const;

    /**
     * @brief Returns pointer to underlying web view.
     *
     * @note Using this method exposes implementation details and is not recommended.
     *
     * @return Pointer to internal web view.
     */
    QWebEngineView* internalWebEngineView() const;

signals:

    /**
     * @brief Signal is emitted when document has been loaded.
     */
    void pdfDocumentLoaded();

private slots:

    void onLoadFinished(bool status);
    void renderPdfData();
    void renderPdfFile(const QString& file);
    void onRenderPdfFinished();

private:
    void setFindFieldText(const QString& text);

    /// Private members.
    QPdfWidgetPrivate* m;
};

#endif // QPDFWIDGET_H
