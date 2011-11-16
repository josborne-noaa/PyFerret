/**
 *  This software was developed by the Thermal Modeling and Analysis
 *  Project(TMAP) of the National Oceanographic and Atmospheric
 *  Administration's (NOAA) Pacific Marine Environmental Lab(PMEL),
 *  hereafter referred to as NOAA/PMEL/TMAP.
 *
 *  Access and use of this software shall impose the following
 *  obligations and understandings on the user. The user is granted the
 *  right, without any fee or cost, to use, copy, modify, alter, enhance
 *  and distribute this software, and any derivative works thereof, and
 *  its supporting documentation for any purpose whatsoever, provided
 *  that this entire notice appears in all copies of the software,
 *  derivative works and supporting documentation.  Further, the user
 *  agrees to credit NOAA/PMEL/TMAP in any publications that result from
 *  the use of this software or in any product that includes this
 *  software. The names TMAP, NOAA and/or PMEL, however, may not be used
 *  in any advertising or publicity to endorse or promote any products
 *  or commercial entity unless specific written permission is obtained
 *  from NOAA/PMEL/TMAP. The user also understands that NOAA/PMEL/TMAP
 *  is not obligated to provide the user with any support, consulting,
 *  training or assistance of any kind with regard to the use, operation
 *  and performance of this software nor to provide the user with any
 *  updates, revisions, new versions or "bug fixes".
 *
 *  THIS SOFTWARE IS PROVIDED BY NOAA/PMEL/TMAP "AS IS" AND ANY EXPRESS
 *  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL NOAA/PMEL/TMAP BE LIABLE FOR ANY SPECIAL,
 *  INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 *  RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 *  CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 *  CONNECTION WITH THE ACCESS, USE OR PERFORMANCE OF THIS SOFTWARE. 
 */
package gov.noaa.pmel.ferret.threddsBrowser;

import thredds.catalog.InvDatasetImpl;

/**
 * An InvDatasetImpl that overrides hasNestedDatasets to always return true.
 * The only purpose of this class is to create an InvDatasetImpl that will
 * always be shown as a folder in a {@link thredds.catalog.ui.CatalogTreeView}.
 * @author Karl M. Smith - karl.smith (at) noaa.gov
 */
public class LocalDirInvDatasetImpl extends InvDatasetImpl {

	/**
	 * Creates an InvDatsetImpl with null for the dataType and the urlPath.
	 * The hasNestedDatasets method of this InvDatasetImpl is overridden to
	 * always return true. 
	 * @param parent parent dataset
	 * @param name display name of the dataset
	 * @param serviceName default service (may be null)
	 */
	public LocalDirInvDatasetImpl(InvDatasetImpl parent, String name, String serviceName) {
		super(parent, name, null, serviceName, null);
	}

	/**
	 * Always returns true so this dataset will be displayed as a folder in
	 * a {@link thredds.catalog.ui.CatalogTreeView}.
	 * @return true
	 */
	@Override
	public boolean hasNestedDatasets() {
		return true;
	}

}