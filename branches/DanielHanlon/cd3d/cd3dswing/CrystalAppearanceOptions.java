/* This class implements a data structure which provides information
 * for the CrystalAppearance class regarding which options
 * are desirable for a particular object. It is also responsible
 * for instatiating the object required to produce such appearance
 * choices and setting their relevant capabilitiy bits. */
import javax.vecmath.*;
import javax.media.j3d.*;

public class CrystalAppearanceOptions{

  Color3f color = new Color3f( 0.4f, 0.8f, 0.9f );

  /* All the following are created false%%\ldots%% */

  /* Choices for transparency. */
  TransparencyAttributes transparency;
  boolean allowsTransparencyAttributesChange;

  boolean allowsTransparencyValueChange;
  boolean allowsTransparencyModeChange;

  /* Choices for simple colouring. */
  ColoringAttributes coloring;
  boolean allowsColoringAttributesChange;

  boolean allowsColorChange;
  boolean allowsShadeModelChange;

  /* Choices for points. */
  PointAttributes points;
  boolean allowsPointAttributesChange;

  boolean allowsPointSizeChange;
  boolean allowsPointAntiAliasingChange;

  /* Choices for lines. */
  LineAttributes lines;
  boolean allowsLineAttributesChange;

  boolean allowsLineWidthChange;
  boolean allowsLinePatternChange;
  boolean allowsLineAntiAliasingChange;

  /* Choices for polygons. */
  PolygonAttributes polygons;
  boolean allowsPolygonAttributesChange;

  boolean allowsCullFaceChange;
  boolean allowsPolygonModeChange;
  boolean allowsPolygonOffsetChange;
  boolean allowsBackFaceNormalFlipChange;

  /* Choice for material colour change. */
  Material material;
  boolean allowsMaterialChange;

  boolean allowsMaterialColorChange;
  boolean allowsShininessChange;

  /* Also possibilities for:
    RenderingAttributes
    TextureAttributes
    Sound
    etc%%\ldots%% could be added.
  */

  /* Set the groups of capability bits that refer to the
   * various objects describing the appearance. */
  public void setCapabilities(){
    if ( allowsTransparencyAttributesChange )
      setTransparencyAttributesCapabilities();
    if ( allowsColoringAttributesChange )
      setColoringAttributesCapabilities();
    if ( allowsPointAttributesChange )
      setPointAttributesCapabilities();
    if ( allowsLineAttributesChange )
      setLineAttributesCapabilities();
    if ( allowsPolygonAttributesChange )
      setPolygonAttributesCapabilities();
    if ( allowsMaterialChange )
      setMaterialCapabilities();
  }

  /*Set the capability bits referring to the transparency. */
  public void setTransparencyAttributesCapabilities(){
    transparency = new TransparencyAttributes();
    transparency.setTransparency( 0.7f );
    /* Initially transparency turned off by default. */
    transparency.setTransparencyMode( TransparencyAttributes.NONE );
    if ( allowsTransparencyValueChange ){
      transparency.setCapability( TransparencyAttributes.ALLOW_VALUE_READ );
      transparency.setCapability( TransparencyAttributes.ALLOW_VALUE_WRITE );
    }
    if ( allowsTransparencyModeChange ){
      transparency.setCapability( TransparencyAttributes.ALLOW_MODE_READ );
      transparency.setCapability( TransparencyAttributes.ALLOW_MODE_WRITE );
    }
  }

  /* Set the capability bits referring to the Simple Colouring. */
  public void setColoringAttributesCapabilities(){
    coloring = new ColoringAttributes();
    coloring.setColor( color );
    if ( allowsColorChange ){
      coloring.setCapability( ColoringAttributes.ALLOW_COLOR_READ );
      coloring.setCapability( ColoringAttributes.ALLOW_COLOR_WRITE );
    }
    if ( allowsShadeModelChange ){
      coloring.setCapability( ColoringAttributes.ALLOW_SHADE_MODEL_READ );
      coloring.setCapability( ColoringAttributes.ALLOW_SHADE_MODEL_WRITE );
    }
  }

  /* Set the capability bits referring to the Point Attributes. */
  public void setPointAttributesCapabilities(){
    points = new PointAttributes();
    if ( allowsPointSizeChange ){
      points.setCapability( PointAttributes.ALLOW_SIZE_READ );
      points.setCapability( PointAttributes.ALLOW_SIZE_WRITE );
    }
    if ( allowsPointAntiAliasingChange ){
      points.setCapability( PointAttributes.ALLOW_ANTIALIASING_READ );
      points.setCapability( PointAttributes.ALLOW_ANTIALIASING_WRITE );
    }
  }

  /* Set the capability bits referring to the Line Attributes. */
  public void setLineAttributesCapabilities(){
    lines = new LineAttributes();
    if ( allowsLineWidthChange ){
      lines.setCapability( LineAttributes.ALLOW_WIDTH_READ );
      lines.setCapability( LineAttributes.ALLOW_WIDTH_WRITE );
    }
    if ( allowsLinePatternChange ){
      lines.setCapability( LineAttributes.ALLOW_PATTERN_READ );
      lines.setCapability( LineAttributes.ALLOW_PATTERN_WRITE );
    }
    if ( allowsLineAntiAliasingChange ){
      lines.setCapability( LineAttributes.ALLOW_ANTIALIASING_READ );
      lines.setCapability( LineAttributes.ALLOW_ANTIALIASING_WRITE );
    }
  }

  /* Set the capability bits referring to the Polygon Attributes. */
  public void setPolygonAttributesCapabilities(){
    polygons = new PolygonAttributes();
    polygons.setCullFace( PolygonAttributes.CULL_NONE );
    polygons.setPolygonMode( PolygonAttributes.POLYGON_FILL );
    polygons.setBackFaceNormalFlip( true );
    if ( allowsCullFaceChange ){
      polygons.setCapability( PolygonAttributes.ALLOW_CULL_FACE_READ );
      polygons.setCapability( PolygonAttributes.ALLOW_CULL_FACE_WRITE );
    }
    if ( allowsPolygonModeChange ){
      polygons.setCapability( PolygonAttributes.ALLOW_MODE_READ );
      polygons.setCapability( PolygonAttributes.ALLOW_MODE_WRITE );
    }
    if ( allowsPolygonOffsetChange ){
      polygons.setCapability( PolygonAttributes.ALLOW_OFFSET_READ );
      polygons.setCapability( PolygonAttributes.ALLOW_OFFSET_WRITE );
    }
    if ( allowsBackFaceNormalFlipChange ){
      polygons.setCapability( PolygonAttributes.ALLOW_NORMAL_FLIP_READ );
      polygons.setCapability( PolygonAttributes.ALLOW_NORMAL_FLIP_WRITE );
    }
  }

  /* Set the capability bits referring to the Material. */
  public void setMaterialCapabilities(){
    material = new Material();
    material.setAmbientColor( new Color3f( color ) );
    material.setEmissiveColor( 0.0f, 0.0f, 0.0f );
    material.setDiffuseColor( (float)( 0.9 * color.x ),
                              (float)( 0.9 * color.y ),
                              (float)( 0.9 * color.z ) );
    material.setSpecularColor( 1.0f, 1.0f, 1.0f );
    material.setShininess( 64f );
    /* Only one capability bit pair for this object for some reason. */
    material.setCapability( Material.ALLOW_COMPONENT_READ );
    material.setCapability( Material.ALLOW_COMPONENT_WRITE );
  }

  /* Choose a colour for use in creating default appearances.
   * %%\textbf{c}%%: An initial colour for the feature. */
  public void setColor( Color3f c ){
    color = c;
  }

  /* Setup some arbitrary defaults for a 2 dimensionsal feature. */
  public void default2D(){
    allowColor();
    allowTransparent();
  }

  /* Setup some arbitrary defaults for a 3 dimensionsal feature. */
  public void default3D(){
    allowMaterial();
    allowTransparent();
    allowPolygonAttributes();
    allowLineAttributes();
    allowPointAttributes();
  }

  /* Setup default options for a feature which allow the color
   *  to be altered. */
  public void allowColor(){
    allowsColoringAttributesChange = true;
    allowsColorChange = true;
    //    allowsShadeModelChange = true;
  }

  /* Setup default options for a feature which allows the material
   * to be altered. */
  public void allowMaterial(){
    allowsMaterialChange = true;
    allowsMaterialColorChange = true;
    allowsShininessChange = true;
  }

  /* Setup default options for a feature which allows the display
   * of polygons to be altered. */
  public void allowPolygonAttributes(){
    allowsPolygonAttributesChange = true;
    allowsCullFaceChange = true;
    allowsPolygonModeChange = true;
    allowsPolygonOffsetChange = true;
    allowsBackFaceNormalFlipChange = true;
  }

  /* Setup default options for a feature which allows the display
   * of lines to be altered. */
  public void allowLineAttributes(){
    allowsLineAttributesChange = true;
    allowsLineWidthChange = true;
    allowsLinePatternChange = true;
    allowsLineAntiAliasingChange = true;
  }

  /* Setup default options for a feature which allows the display
   * of points to be altered. */
  public void allowPointAttributes(){
    allowsPointAttributesChange = true;
    allowsPointSizeChange = true;
    allowsPointAntiAliasingChange= true;
  }

  /*Setup default options for a feature which may be made
   * transparent. */
  public void allowTransparent(){
    allowsTransparencyAttributesChange = true;
    allowsTransparencyValueChange = true;
    allowsTransparencyModeChange = true;

  }
}

