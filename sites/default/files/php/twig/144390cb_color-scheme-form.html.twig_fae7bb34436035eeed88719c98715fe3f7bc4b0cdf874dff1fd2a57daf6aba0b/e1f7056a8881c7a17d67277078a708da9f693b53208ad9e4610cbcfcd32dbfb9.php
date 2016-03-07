<?php

/* core/modules/color/templates/color-scheme-form.html.twig */
class __TwigTemplate_fae7bb34436035eeed88719c98715fe3f7bc4b0cdf874dff1fd2a57daf6aba0b extends Twig_Template
{
    public function __construct(Twig_Environment $env)
    {
        parent::__construct($env);

        $this->parent = false;

        $this->blocks = array(
        );
    }

    protected function doDisplay(array $context, array $blocks = array())
    {
        // line 18
        echo "<div class=\"color-form clearfix\">
  ";
        // line 19
        echo $this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["form"]) ? $context["form"] : null), "scheme", array()), "html", null, true);
        echo "
  <div class=\"clearfix color-palette js-color-palette\">
    ";
        // line 21
        echo $this->env->getExtension('drupal_core')->escapeFilter($this->env, $this->getAttribute((isset($context["form"]) ? $context["form"] : null), "palette", array()), "html", null, true);
        echo "
  </div>
  ";
        // line 23
        echo $this->env->getExtension('drupal_core')->escapeFilter($this->env, twig_without((isset($context["form"]) ? $context["form"] : null), "scheme", "palette"), "html", null, true);
        echo "
  <h2>";
        // line 24
        echo $this->env->getExtension('drupal_core')->renderVar(t("Preview"));
        echo "</h2>
  ";
        // line 25
        echo $this->env->getExtension('drupal_core')->escapeFilter($this->env, (isset($context["html_preview"]) ? $context["html_preview"] : null), "html", null, true);
        echo "
</div>
";
    }

    public function getTemplateName()
    {
        return "core/modules/color/templates/color-scheme-form.html.twig";
    }

    public function isTraitable()
    {
        return false;
    }

    public function getDebugInfo()
    {
        return array (  40 => 25,  36 => 24,  32 => 23,  27 => 21,  22 => 19,  19 => 18,);
    }
}
